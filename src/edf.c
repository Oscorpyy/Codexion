/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 16:18:07 by opernod           #+#    #+#             */
/*   Updated: 2026/05/06 17:50:26 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

long	get_time(void);
int		check_running(t_all *all);
void	print_state(t_coder *coder, char *state);
void	wait_for_dongle(t_all *all, int idx, int *flag);
void	release_dongle(t_all *all, int idx, int *flag);
void	set_order(t_coder *c, int l, int r, t_order *o);

static int	edf_prio(t_coder *c1, t_coder *c2)
{
	long	t1;
	long	t2;

	t1 = c1->last_compile_time;
	t2 = c2->last_compile_time;
	if (c1->compiles_done != -1
		&& c1->compiles_done >= c1->all->args->number_of_compiles_required
		&& c1->all->args->number_of_compiles_required != -1)
		return (0);
	if (t1 == t2)
		return (c1->id < c2->id);
	return (t1 < t2);
}

static void	wait_dongle_edf(t_all *a, t_coder *c, int t_d, int *f)
{
	t_coder	*o;
	long	cd;

	o = &a->coders[t_d];
	if (t_d == c->id - 1)
		o = &a->coders[(t_d - 1 + a->args->number_of_coders)
			% a->args->number_of_coders];
	while (check_running(a))
	{
		pthread_mutex_lock(&a->cooldown_mutex);
		cd = a->dongle_cooldown_end[t_d];
		pthread_mutex_unlock(&a->cooldown_mutex);
		if (get_time() < cd && usleep(500) == 0)
			continue ;
		if (edf_prio(o, c) && usleep(100) == 0)
			continue ;
		if (*f == 0 && pthread_mutex_trylock(&a->dongle_mutexes[t_d]) == 0)
			*f = 1;
		if (*f == 1)
			break ;
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
	print_state(c, "has taken a dongle");
	wait_dongle_edf(a, c, o.second, o.s_flag);
	if (!check_running(a))
	{
		release_dongle(a, o.first, o.f_flag);
		release_dongle(a, o.second, o.s_flag);
		return (0);
	}
	print_state(c, "has taken a dongle");
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
	print_state(c, "is compiling");
	pthread_mutex_lock(&c->coder_mutex);
	c->last_compile_time = get_time();
	pthread_mutex_unlock(&c->coder_mutex);
	ft_usleep(a->args->time_to_compile, c);
	release_both(a, c, l, r);
	pthread_mutex_lock(&c->coder_mutex);
	c->compiles_done++;
	print_state(c, "is debugging");
	ft_usleep(a->args->time_to_debug, c);
	if (check_burnout(a, a->coders, c->id - 1, get_time()))
		return (0);
	print_state(c, "is refactoring");
	ft_usleep(a->args->time_to_refactor, c);
	if (a->args->number_of_compiles_required != -1
		&& c->compiles_done >= a->args->number_of_compiles_required)
	{
		c->last_compile_time = 2147483647;
		pthread_mutex_unlock(&c->coder_mutex);
		return (0);
	}
	pthread_mutex_unlock(&c->coder_mutex);
	return (1);
}
