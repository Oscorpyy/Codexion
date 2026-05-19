/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:59:33 by opernod           #+#    #+#             */
/*   Updated: 2026/05/19 17:15:08 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

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
	}
	i = -1;
	while (++i < all->args->number_of_coders)
	{
		if (pthread_create(&c[i].thread_id, NULL, coder_routine, &c[i]) != 0)
			return (i);
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
			return (1);
		}
		all->is_running = 0;
		pthread_mutex_unlock(&all->run_mutex);
		print_state(&coders[i], "burned out", 1);
		return (1);
	}
	return (0);
}

int	check_compile_limit(t_all *a, t_coder *c)
{
	if (a->args->number_of_compiles_required > 0
		&& c->compiles_done >= a->args->number_of_compiles_required)
	{
		pthread_mutex_lock(&c->coder_mutex);
		c->last_compile_time = 2147483647;
		pthread_mutex_unlock(&c->coder_mutex);
		return (0);
	}
	return (1);
}

static void	start_simulation(t_all *a, t_coder *co, pthread_mutex_t *m)
{
	pthread_t	monitor_thread;

	if (init_threads(m, co, a) == a->args->number_of_coders)
	{
		if (pthread_create(&monitor_thread, NULL, monitor_routine, a) != 0)
		{
			pthread_mutex_lock(&a->run_mutex);
			a->is_running = 0;
			pthread_mutex_unlock(&a->run_mutex);
		}
		else
			pthread_join(monitor_thread, NULL);
	}
	else
		a->is_running = 0;
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
	start_simulation(a, co, &m);
	cleanup(args, a, co, &m);
	return (0);
}
