/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 16:18:07 by opernod           #+#    #+#             */
/*   Updated: 2026/05/19 17:15:08 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

static int	edf_tie_break(t_coder *c1, t_coder *c2, int c1_done, int c2_done)
{
	if (c1_done == 0 && c2_done == 0)
	{
		if (c1->id % 2 != 0 && c2->id % 2 == 0)
			return (1);
		if (c2->id % 2 != 0 && c1->id % 2 == 0)
			return (0);
	}
	return (c1->id < c2->id);
}

static int	edf_prio(t_coder *c1, t_coder *c2)
{
	long	t1;
	long	t2;
	int		c1_done;
	int		c2_done;

	if (c1 == c2 || c2->has_left || c2->has_right)
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
		return (edf_tie_break(c1, c2, c1_done, c2_done));
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

int	compile_cycle(t_all *a, t_coder *c, int l, int r)
{
	t_order	o;

	print_state(c, "is compiling", 0);
	pthread_mutex_lock(&c->coder_mutex);
	c->last_compile_time = get_time();
	pthread_mutex_unlock(&c->coder_mutex);
	ft_usleep(a->args->time_to_compile, c);
	set_order(c, l, r, &o);
	release_dongle(a, o.second, o.s_flag);
	release_dongle(a, o.first, o.f_flag);
	pthread_mutex_lock(&c->coder_mutex);
	c->compiles_done++;
	pthread_mutex_unlock(&c->coder_mutex);
	print_state(c, "is debugging", 0);
	ft_usleep(a->args->time_to_debug, c);
	if (check_burnout(a, a->coders, c->id - 1, get_time()))
		return (0);
	print_state(c, "is refactoring", 0);
	ft_usleep(a->args->time_to_refactor, c);
	if (!check_compile_limit(a, c))
		return (0);
	return (1);
}
