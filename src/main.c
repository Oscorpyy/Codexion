/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:59:33 by opernod           #+#    #+#             */
/*   Updated: 2026/04/29 13:43:53 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	if (strcmp(coder->all->args->scheduler, "fifo") == 0)
		acquire_dongles_fifo(coder->all, coder);
	else
		acquire_dongles_edf(coder->all, coder);
	return (NULL);
}

int	init_threads(pthread_mutex_t *mut, t_coder *c, t_all *all)
{
	int	i;

	i = -1;
	all->start_time = get_time();
	while (++i < all->args->number_of_coders)
	{
		c[i].id = i + 1;
		c[i].write_mutex = mut;
		c[i].all = all;
		c[i].last_compile_time = all->start_time;
		c[i].compiles_done = 0;
		if (pthread_mutex_init(&c[i].coder_mutex, NULL) != 0)
			return (i);
		if (pthread_create(&c[i].thread_id, NULL, coder_routine, &c[i]) != 0)
		{
			pthread_mutex_destroy(&c[i].coder_mutex);
			return (i);
		}
	}
	return (i);
}

int	check_burnout(t_all *all, t_coder *coders, int i, long current_time)
{
	if (current_time - coders[i].last_compile_time > all->args->time_to_burnout)
	{
		pthread_mutex_lock(&all->run_mutex);
		if (!all->is_running)
		{
			pthread_mutex_unlock(&all->run_mutex);
			pthread_mutex_unlock(&coders[i].coder_mutex);
			return (1);
		}
		all->is_running = 0;
		pthread_mutex_unlock(&all->run_mutex);
		pthread_mutex_lock(coders[i].write_mutex);
		printf("%ld %d burned out\n", current_time - all->start_time,
			coders[i].id);
		pthread_mutex_unlock(coders[i].write_mutex);
		pthread_mutex_unlock(&coders[i].coder_mutex);
		return (1);
	}
	return (0);
}

void	monitor_routine(t_all *all, t_coder *coders, int all_c)
{
	int	i;

	while (check_running(all) && usleep(50) == 0)
	{
		i = -1;
		all_c = 1;
		while (++i < all->args->number_of_coders)
		{
			pthread_mutex_lock(&coders[i].coder_mutex);
			if (check_burnout(all, coders, i, get_time()))
				return ;
			if (all->args->number_of_compiles_required == -1
				|| coders[i].compiles_done
				< all->args->number_of_compiles_required)
				all_c = 0;
			pthread_mutex_unlock(&coders[i].coder_mutex);
		}
		if (all->args->number_of_compiles_required != -1 && all_c)
		{
			pthread_mutex_lock(&all->run_mutex);
			all->is_running = 0;
			pthread_mutex_unlock(&all->run_mutex);
			return ;
		}
	}
}

int	main(int argc, char **argv)
{
	t_args			*args;
	t_coder			*co;
	pthread_mutex_t	m;
	t_all			*a;

	a = calloc(1, sizeof(t_all));
	args = calloc(1, sizeof(t_args));
	if (!args || !a || parssing(args, argc, argv))
		return (free_all(args, a, NULL), 1);
	if (args->number_of_coders <= 0 || args->number_of_compiles_required == 0)
		return (free_all(args, a, NULL), 0);
	co = calloc(1, sizeof(t_coder) * args->number_of_coders);
	if (!co || setup_mutex(&m, a, args, co))
		return (free_all(args, a, co), 1);
	if (init_threads(&m, co, a) == args->number_of_coders)
		monitor_routine(a, co, 0);
	else
		a->is_running = 0;
	cleanup(args, a, co, &m);
	return (0);
}
