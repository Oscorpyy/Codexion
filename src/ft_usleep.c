/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_usleep.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 18:09:54 by opernod           #+#    #+#             */
/*   Updated: 2026/05/12 11:25:26 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

void	ft_usleep(long time_in_ms, t_coder *coder)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < time_in_ms && check_running(coder->all))
	{
		usleep(500);
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
			break ;
		pthread_mutex_unlock(&coder->coder_mutex);
	}
}

t_coder	*get_opponent(t_all *a, t_coder *c, int t_d)
{
	int	n;

	n = a->args->number_of_coders;
	if (t_d == c->id - 1)
		return (&a->coders[(t_d - 1 + n) % n]);
	return (&a->coders[t_d]);
}
