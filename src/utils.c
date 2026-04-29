/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 14:52:57 by opernod           #+#    #+#             */
/*   Updated: 2026/04/29 16:19:43 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

long	get_time(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

int	check_running(t_all *all)
{
	int	status;

	pthread_mutex_lock(&all->run_mutex);
	status = all->is_running;
	pthread_mutex_unlock(&all->run_mutex);
	return (status);
}

void	print_state(t_coder *coder, char *state)
{
	long	current_time;

	pthread_mutex_lock(coder->write_mutex);
	if (check_running(coder->all))
	{
		current_time = get_time() - coder->all->start_time;
		printf("%ld %d %s\n", current_time, coder->id, state);
	}
	pthread_mutex_unlock(coder->write_mutex);
}

static int	init_dongles(t_all *all, t_args *arg)
{
	int	i;

	all->dongle_mutexes = malloc(sizeof(pthread_mutex_t)
			* arg->number_of_coders);
	all->dongle_cooldown_end = calloc(arg->number_of_coders, sizeof(long));
	if (!all->dongle_mutexes || !all->dongle_cooldown_end
		|| pthread_mutex_init(&all->cooldown_mutex, NULL) != 0)
		return (1);
	i = -1;
	while (++i < arg->number_of_coders)
		pthread_mutex_init(&all->dongle_mutexes[i], NULL);
	return (0);
}

int	setup_mutex(pthread_mutex_t *mutex, t_all *all, t_args *arg, t_coder *c)
{
	if (pthread_mutex_init(mutex, NULL) != 0
		|| pthread_mutex_init(&all->run_mutex, NULL) != 0)
	{
		printf("Error: mutex initialization failed\n");
		free_all(arg, all, c);
		return (1);
	}
	all->args = arg;
	all->is_running = 1;
	all->coders = c;
	if (init_dongles(all, arg) != 0)
	{
		pthread_mutex_destroy(&all->run_mutex);
		pthread_mutex_destroy(mutex);
		free_all(arg, all, c);
		return (1);
	}
	return (0);
}
