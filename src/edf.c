/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 16:18:07 by opernod           #+#    #+#             */
/*   Updated: 2026/05/19 14:25:48 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

static int	edf_prio(t_coder *c1, t_coder *c2)
{
	long	t1;
	long	t2;
	int		c1_done;
	int		c2_done;

	if (c1 == c2)
		return (0);
	if (c2->has_left || c2->has_right)
		return (0);
	mutex_lock_ordered(&c1->coder_mutex, &c2->coder_mutex, c1->id, c2->id);
	t1 = c1->last_compile_time;
	c1_done = c1->compiles_done;
	t2 = c2->last_compile_time;
	c2_done = c2->compiles_done;
	pthread_mutex_unlock(&c1->coder_mutex);
	pthread_mutex_unlock(&c2->coder_mutex);
	if (c2->all->args->number_of_compiles_required > 0
		&& c1_done >= c2->all->args->number_of_compiles_required)
		return (0);
	if (c1_done != c2_done)
		return (c1_done < c2_done);
	if (t1 == t2)
		return (c1->id > c2->id);
	return (t1 < t2);
}

static void	wait_dongle_edf(t_all *a, t_coder *c, int t_d, int *flag)
{
	t_coder	*o;

	o = get_opponent(a, c, t_d);
	while (check_running(a))
	{
		if (a->args->number_of_coders > 1 && o && edf_prio(o, c))
		{
			ft_usleep(1, c);
			continue ;
		}
		if (try_take_dongle(a, t_d, flag))
			return ;
		ft_usleep(1, c);
	}
}

int	take_dongles_edf(t_all *a, t_coder *c, int l, int r)
{
	t_order	o;

	set_order(c, l, r, &o);
	wait_dongle_edf(a, c, o.first, o.f_flag);
	if (!check_running(a))
	{
		release_dongle(a, o.first, o.f_flag);
		return (0);
	}
	print_state(c, "has taken a dongle", 0);
	wait_dongle_edf(a, c, o.second, o.s_flag);
	if (!check_running(a))
	{
		release_dongle(a, o.second, o.s_flag);
		release_dongle(a, o.first, o.f_flag);
		return (0);
	}
	print_state(c, "has taken a dongle", 0);
	return (1);
}

static void	release_both(t_all *a, t_coder *c, int l, int r)
{
	int	first;
	int	second;
	int	*f_flag;
	int	*s_flag;

	if (l < r)
	{
		first = l;
		f_flag = &c->has_left;
		second = r;
		s_flag = &c->has_right;
	}
	else
	{
		first = r;
		f_flag = &c->has_right;
		second = l;
		s_flag = &c->has_left;
	}
	release_dongle(a, second, s_flag);
	release_dongle(a, first, f_flag);
}

int	compile_cycle(t_all *a, t_coder *c, int l, int r)
{
	print_state(c, "is compiling", 0);
	pthread_mutex_lock(&c->coder_mutex);
	c->last_compile_time = get_time();
	pthread_mutex_unlock(&c->coder_mutex);
	ft_usleep(a->args->time_to_compile, c);
	release_both(a, c, l, r);
	pthread_mutex_lock(&c->coder_mutex);
	c->compiles_done++;
	pthread_mutex_unlock(&c->coder_mutex);
	print_state(c, "is debugging", 0);
	ft_usleep(a->args->time_to_debug, c);
	if (check_burnout(a, a->coders, c->id - 1, get_time()))
		return (0);
	print_state(c, "is refactoring", 0);
	ft_usleep(a->args->time_to_refactor, c);
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
