/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 16:18:07 by opernod           #+#    #+#             */
/*   Updated: 2026/04/30 17:37:51 by opernod          ###   ########lyon.fr   */
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

static int	take_dongles_edf(t_all *a, t_coder *c, int l, int r)
{
	t_order	o;

	set_order(c, l, r, &o);
	wait_for_dongle(a, o.first, o.f_flag);
	if (!check_running(a))
	{
		release_dongle(a, o.first, o.f_flag);
		return (0);
	}
	print_state(c, "has taken a dongle");
	wait_for_dongle(a, o.second, o.s_flag);
	if (!check_running(a))
	{
		release_dongle(a, o.second, o.s_flag);
		release_dongle(a, o.first, o.f_flag);
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

static int	compile_cycle(t_all *a, t_coder *c, int l, int r)
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
		pthread_mutex_unlock(&c->coder_mutex);
		return (0);
	}
	pthread_mutex_unlock(&c->coder_mutex);
	return (1);
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
