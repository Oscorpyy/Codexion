/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 10:00:00 by opernod           #+#    #+#             */
/*   Updated: 2026/04/24 12:08:25 by opernod          ###   ########lyon.fr   */
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

void	free_all(t_args *args, t_all *all, t_coder *coder)
{
	if (args)
		free(args);
	if (all)
		free(all);
	if (coder)
		free(coder);
}

int	setup_mutex(pthread_mutex_t *mutex, t_all *all, t_args *args,
		t_coder *coder)
{
	if (pthread_mutex_init(mutex, NULL) != 0)
	{
		printf("Error: mutex initialization failed\n");
		free_all(args, all, coder);
		return (1);
	}
	all->args = args;
	all->is_running = 1;
	if (pthread_mutex_init(&all->run_mutex, NULL) != 0)
	{
		printf("Error: run_mutex init failed\n");
		pthread_mutex_destroy(mutex);
		free_all(args, all, coder);
		return (1);
	}
	all->coders = coder;
	return (0);
}
