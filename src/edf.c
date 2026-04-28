/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 10:00:00 by opernod           #+#    #+#             */
/*   Updated: 2026/04/28 20:00:00 by opernod          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

long	get_time(void);
int		check_running(t_all *all);
void	print_state(t_coder *coder, char *state);

static void	wait_for_dongle(t_all *all, int idx)
{
	while (check_running(all))
	{
		if (get_time() >= all->dongle_cooldown_end[idx])
			if (pthread_mutex_trylock(&all->dongle_mutexes[idx]) == 0)
				return ;
		usleep(100);
	}
}

static void	release_dongle(t_all *all, int idx)
{
	pthread_mutex_unlock(&all->dongle_mutexes[idx]);
	pthread_mutex_lock(&all->cooldown_mutex);
	all->dongle_cooldown_end[idx] = get_time() + all->args->dongle_cooldown;
	pthread_mutex_unlock(&all->cooldown_mutex);
}

static int	take_dongles_edf(t_all *a, t_coder *c, int l, int r)
{
	int	first = (l < r) ? l : r;
	int	second = (l < r) ? r : l;

	wait_for_dongle(a, first);
	print_state(c, "has taken a dongle");
	if (a->args->number_of_coders == 1)
	{
		ft_usleep(a->args->time_to_burnout + 10, c);
		release_dongle(a, first);
		return (0);
	}
	wait_for_dongle(a, second);
	print_state(c, "has taken a dongle");
	return (1);
}

static void	release_both(t_all *a, int l, int r)
{
	int	first = (l < r) ? l : r;
	int	second = (l < r) ? r : l;

	release_dongle(a, second);
	release_dongle(a, first);
}

static int	compile_cycle(t_all *a, t_coder *c, int l, int r)
{
	print_state(c, "is compiling");
	pthread_mutex_lock(&c->coder_mutex);
	c->last_compile_time = get_time();
	pthread_mutex_unlock(&c->coder_mutex);
	ft_usleep(a->args->time_to_compile, c);
	release_both(a, l, r);
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

void	acquire_dongles_edf(t_all *all, t_coder *coder)
{
	int	left = coder->id - 1;
	int	right = coder->id % all->args->number_of_coders;

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
