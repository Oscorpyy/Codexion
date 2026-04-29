/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 16:19:16 by opernod           #+#    #+#             */
/*   Updated: 2026/04/29 16:19:55 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
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

void	wait_for_dongle(t_all *all, int idx)
{
	while (check_running(all))
	{
		pthread_mutex_lock(&all->cooldown_mutex);
		if (get_time() >= all->dongle_cooldown_end[idx])
		{
			pthread_mutex_unlock(&all->cooldown_mutex);
			if (pthread_mutex_trylock(&all->dongle_mutexes[idx]) == 0)
				return ;
		}
		else
			pthread_mutex_unlock(&all->cooldown_mutex);
		usleep(100);
	}
}

void	release_dongle(t_all *all, int idx)
{
	pthread_mutex_unlock(&all->dongle_mutexes[idx]);
	pthread_mutex_lock(&all->cooldown_mutex);
	all->dongle_cooldown_end[idx] = get_time() + all->args->dongle_cooldown;
	pthread_mutex_unlock(&all->cooldown_mutex);
}
