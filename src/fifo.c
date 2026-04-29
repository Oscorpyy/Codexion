/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fifo.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 15:16:10 by opernod           #+#    #+#             */
/*   Updated: 2026/04/29 15:16:12 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

/* Prototypes des fonctions externes */
long	get_time(void);
int		check_running(t_all *all);
void	print_state(t_coder *coder, char *state);
void	wait_for_dongle(t_all *all, int idx);
void	release_dongle(t_all *all, int idx);
void	ft_usleep(long time, t_coder *coder);
int		check_burnout(t_all *a, t_coder *c, int i, long t);

static void	acquire_dongles(t_all *a, t_coder *c, int l, int r)
{
	int	first;
	int	second;

	first = l;
	second = r;
	if (l > r)
	{
		first = r;
		second = l;
	}
	wait_for_dongle(a, first);
	print_state(c, "has taken a dongle");
	if (a->args->number_of_coders != 1)
	{
		wait_for_dongle(a, second);
		print_state(c, "has taken a dongle");
	}
}

static void	release_dongles(t_all *a, int l, int r)
{
	int	first;
	int	second;

	first = l;
	second = r;
	if (l > r)
	{
		first = r;
		second = l;
	}
	if (a->args->number_of_coders != 1)
		release_dongle(a, second);
	release_dongle(a, first);
}

static int	do_cycle(t_all *a, t_coder *c)
{
	print_state(c, "is compiling");
	pthread_mutex_lock(&c->coder_mutex);
	c->last_compile_time = get_time();
	pthread_mutex_unlock(&c->coder_mutex);
	ft_usleep(a->args->time_to_compile, c);
	pthread_mutex_lock(&c->coder_mutex);
	c->compiles_done++;
	if (a->args->number_of_compiles_required != -1
		&& c->compiles_done >= a->args->number_of_compiles_required)
	{
		pthread_mutex_unlock(&c->coder_mutex);
		return (0);
	}
	pthread_mutex_unlock(&c->coder_mutex);
	print_state(c, "is debugging");
	ft_usleep(a->args->time_to_debug, c);
	print_state(c, "is refactoring");
	ft_usleep(a->args->time_to_refactor, c);
	return (1);
}

static int	check_coder_status(t_all *all, t_coder *coder, int l)
{
	pthread_mutex_lock(&coder->coder_mutex);
	if (check_burnout(all, all->coders, l, get_time()))
	{
		pthread_mutex_unlock(&coder->coder_mutex);
		return (1);
	}
	pthread_mutex_unlock(&coder->coder_mutex);
	return (0);
}

void	acquire_dongles_fifo(t_all *all, t_coder *coder)
{
	int	l;
	int	r;

	l = coder->id - 1;
	r = coder->id % all->args->number_of_coders;
	while (check_running(all))
	{
		acquire_dongles(all, coder, l, r);
		if (all->args->number_of_coders == 1)
		{
			ft_usleep(all->args->time_to_burnout + 10, coder);
			release_dongles(all, l, r);
			break ;
		}
		if (!do_cycle(all, coder))
		{
			release_dongles(all, l, r);
			break ;
		}
		release_dongles(all, l, r);
		if (check_coder_status(all, coder, l))
			break ;
	}
}
