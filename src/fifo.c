/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 15:16:10 by opernod           #+#    #+#             */
/*   Updated: 2026/05/13 16:09:34 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

static void	release_dongles(t_all *a, t_coder *c, int l, int r)
{
	t_order	o;

	set_order(c, l, r, &o);
	if (a->args->number_of_coders != 1)
		release_dongle(a, o.second, o.s_flag);
	release_dongle(a, o.first, o.f_flag);
}

static int	do_cycle(t_all *a, t_coder *c, int l, int r)
{
	print_state(c, "is compiling", 0);
	pthread_mutex_lock(&c->coder_mutex);
	c->last_compile_time = get_time();
	pthread_mutex_unlock(&c->coder_mutex);
	ft_usleep(a->args->time_to_compile, c);
	release_dongles(a, c, l, r);
	pthread_mutex_lock(&c->coder_mutex);
	c->compiles_done++;
	print_state(c, "is debugging", 0);
	ft_usleep(a->args->time_to_debug, c);
	print_state(c, "is refactoring", 0);
	ft_usleep(a->args->time_to_refactor, c);
	if (a->args->number_of_compiles_required != -1
		&& c->compiles_done >= a->args->number_of_compiles_required)
	{
		pthread_mutex_unlock(&c->coder_mutex);
		return (0);
	}
	pthread_mutex_unlock(&c->coder_mutex);
	if (check_burnout(a, a->coders, c->id - 1, get_time()))
		return (0);
	return (1);
}

static int	check_coder_status(t_all *all, t_coder *coder, int l)
{
	pthread_mutex_lock(&coder->coder_mutex);
	if (check_burnout(all, all->coders, l, get_time()))
	{
		return (1);
	}
	pthread_mutex_unlock(&coder->coder_mutex);
	return (0);
}

static int	acquire_dongles(t_all *a, t_coder *c, int l, int r)
{
	t_order	o;

	set_order(c, l, r, &o);
	wait_for_dongle(a, o.first, o.f_flag);
	if (!check_running(a))
	{
		release_dongle(a, o.first, o.f_flag);
		return (0);
	}
	print_state(c, "has taken a dongle", 0);
	if (a->args->number_of_coders != 1)
	{
		wait_for_dongle(a, o.second, o.s_flag);
		if (!check_running(a))
		{
			release_dongle(a, o.second, o.s_flag);
			release_dongle(a, o.first, o.f_flag);
			return (0);
		}
		print_state(c, "has taken a dongle", 0);
	}
	return (1);
}

void	acquire_dongles_fifo(t_all *all, t_coder *coder)
{
	int	l;
	int	r;

	l = coder->id - 1;
	r = coder->id % all->args->number_of_coders;
	while (check_running(all))
	{
		if (!acquire_dongles(all, coder, l, r))
			break ;
		if (all->args->number_of_coders == 1)
		{
			ft_usleep(all->args->time_to_burnout + 10, coder);
			release_dongles(all, coder, l, r);
			break ;
		}
		if (!do_cycle(all, coder, l, r))
			break ;
		if (check_coder_status(all, coder, l))
			break ;
	}
}
