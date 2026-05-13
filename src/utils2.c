/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 16:19:16 by opernod           #+#    #+#             */
/*   Updated: 2026/05/13 14:35:55 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#define LOCK 1
#define UNLOCK 0
#include "../includes/codexion.h"

void	free_all(t_args *args, t_all *all, t_coder *coder)
{
	if (args)
		free(args);
	if (all)
		free(all);
	if (coder)
		free(coder);
}

void	wait_for_dongle(t_all *all, int idx, int *flag)
{
	while (check_running(all))
	{
		pthread_mutex_lock(&all->cooldown_mutex);
		if (get_time() >= all->dongle_cooldown_end[idx])
		{
			pthread_mutex_unlock(&all->cooldown_mutex);
			if (*flag == 0
				&& pthread_mutex_trylock(&all->dongle_mutexes[idx]) == 0)
			{
				*flag = 1;
				return ;
			}
			if (*flag == 1)
				return ;
		}
		else
			pthread_mutex_unlock(&all->cooldown_mutex);
		usleep(500);
	}
}

void	release_dongle(t_all *all, int idx, int *flag)
{
	if (flag != NULL && *flag == 1)
	{
		*flag = 0;
		pthread_mutex_unlock(&all->dongle_mutexes[idx]);
		pthread_mutex_lock(&all->cooldown_mutex);
		all->dongle_cooldown_end[idx] = get_time() + all->args->dongle_cooldown;
		pthread_mutex_unlock(&all->cooldown_mutex);
	}
}

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

void	safe_dongle_op(pthread_mutex_t *mutex, int *flag, int action)
{
	if (action == LOCK && *flag == 0)
	{
		if (pthread_mutex_lock(mutex) == 0)
			*flag = 1;
	}
	else if (action == UNLOCK && *flag == 1)
	{
		if (pthread_mutex_unlock(mutex) == 0)
			*flag = 0;
	}
}
