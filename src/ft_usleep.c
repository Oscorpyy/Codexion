/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_usleep.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 18:09:54 by opernod           #+#    #+#             */
/*   Updated: 2026/05/19 17:15:08 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

void	ft_usleep(long time_in_ms, t_coder *coder)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < time_in_ms && check_running(coder->all)
		&& !check_burnout(coder->all, coder->all->coders,
			coder->id - 1, get_time()))
	{
		usleep(50);
	}
}

void	set_order(t_coder *c, int l, int r, t_order *o)
{
	if (l < r)
	{
		o->first = l;
		o->f_flag = &c->has_left;
		o->second = r;
		o->s_flag = &c->has_right;
	}
	else
	{
		o->first = r;
		o->f_flag = &c->has_right;
		o->second = l;
		o->s_flag = &c->has_left;
	}
}

void	acquire_dongles_edf(t_all *all, t_coder *coder)
{
	int	left;
	int	right;

	left = coder->id - 1;
	right = coder->id % all->args->number_of_coders;
	while (check_running(all))
	{
		if (!take_dongles_edf(all, coder, left, right))
			break ;
		if (!compile_cycle(all, coder, left, right))
			break ;
		pthread_mutex_lock(&coder->coder_mutex);
		if (check_burnout(all, all->coders, left, get_time()))
		{
			pthread_mutex_unlock(&coder->coder_mutex);
			break ;
		}
		pthread_mutex_unlock(&coder->coder_mutex);
	}
}

void	mutex_lock_ordered(pthread_mutex_t *m1, pthread_mutex_t *m2,
						int id1, int id2)
{
	if (id1 < id2)
	{
		pthread_mutex_lock(m1);
		pthread_mutex_lock(m2);
	}
	else
	{
		pthread_mutex_lock(m2);
		pthread_mutex_lock(m1);
	}
}
