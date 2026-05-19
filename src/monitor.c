/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/05/19 14:23:45 by opernod           #+#    #+#             */
/*   Updated: 2026/05/19 14:25:48 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

int	try_take_dongle(t_all *a, int t_d, int *flag)
{
	if (*flag == 0 && pthread_mutex_trylock(&a->dongle_mutexes[t_d]) == 0)
	{
		pthread_mutex_lock(&a->cooldown_mutex);
		if (get_time() >= a->dongle_cooldown_end[t_d])
		{
			*flag = 1;
			pthread_mutex_unlock(&a->cooldown_mutex);
			return (1);
		}
		pthread_mutex_unlock(&a->cooldown_mutex);
		pthread_mutex_unlock(&a->dongle_mutexes[t_d]);
	}
	return (*flag == 1);
}

static int	check_single_coder(t_all *all, t_coder *c, int i)
{
	pthread_mutex_lock(&c[i].coder_mutex);
	if (all->args->number_of_compiles_required != -1
		&& c[i].compiles_done >= all->args->number_of_compiles_required)
	{
		pthread_mutex_unlock(&c[i].coder_mutex);
		return (1);
	}
	if (check_burnout(all, c, i, get_time()))
	{
		pthread_mutex_unlock(&c[i].coder_mutex);
		return (-1);
	}
	pthread_mutex_unlock(&c[i].coder_mutex);
	return (0);
}

static int	check_all_coders(t_all *all, t_coder *coders)
{
	int	i;
	int	res;
	int	all_c;

	i = -1;
	all_c = 1;
	while (++i < all->args->number_of_coders && check_running(all))
	{
		res = check_single_coder(all, coders, i);
		if (res == -1)
			return (-1);
		if (res == 0)
			all_c = 0;
	}
	return (all_c);
}

void	*monitor_routine(void *arg_ptr)
{
	t_all	*all;
	int		all_c;

	all = (t_all *)arg_ptr;
	while (check_running(all) && usleep(50) == 0)
	{
		all_c = check_all_coders(all, all->coders);
		if (all_c == -1)
			return (NULL);
		if (all->args->number_of_compiles_required != -1 && all_c)
		{
			pthread_mutex_lock(&all->run_mutex);
			all->is_running = 0;
			pthread_mutex_unlock(&all->run_mutex);
			return (NULL);
		}
	}
	return (NULL);
}
