/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   edf.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 10:00:00 by opernod           #+#    #+#             */
/*   Updated: 2026/04/28 15:51:51 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

long	get_time(void);
int		check_running(t_all *all);
void	print_state(t_coder *coder, char *state);

static int	take_dongles_edf(t_all *all, t_coder *coder, int left, int right)
{
	int	first;
	int	second;

	first = left;
	second = right;
	if (left > right)
	{
		first = right;
		second = left;
	}
	pthread_mutex_lock(&all->dongle_mutexes[first]);
	print_state(coder, "has taken a dongle");
	if (all->args->number_of_coders == 1)
	{
		ft_usleep(all->args->time_to_burnout + 10, coder);
		pthread_mutex_unlock(&all->dongle_mutexes[first]);
		return (0);
	}
	pthread_mutex_lock(&all->dongle_mutexes[second]);
	print_state(coder, "has taken a dongle");
	return (1);
}

static int	compile_routine_edf(t_all *all, t_coder *coder, int l, int r)
{
	if (!take_dongles_edf(all, coder, l, r))
		return (0);
	print_state(coder, "is compiling");
	pthread_mutex_lock(&coder->coder_mutex);
	coder->last_compile_time = get_time();
	pthread_mutex_unlock(&coder->coder_mutex);
	ft_usleep(all->args->time_to_compile, coder);
	if (l < r)
	{
		pthread_mutex_unlock(&all->dongle_mutexes[r]);
		pthread_mutex_unlock(&all->dongle_mutexes[l]);
	}
	else
	{
		pthread_mutex_unlock(&all->dongle_mutexes[l]);
		pthread_mutex_unlock(&all->dongle_mutexes[r]);
	}
	pthread_mutex_lock(&coder->coder_mutex);
	coder->compiles_done++;
	pthread_mutex_unlock(&coder->coder_mutex);
	if (all->args->number_of_compiles_required != -1
		&& coder->compiles_done >= all->args->number_of_compiles_required)
		return (0);
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
		if (!compile_routine_edf(all, coder, left, right))
			break ;
		print_state(coder, "is debugging");
		ft_usleep(all->args->time_to_debug, coder);
		print_state(coder, "is refactoring");
		ft_usleep(all->args->time_to_refactor, coder);
		pthread_mutex_lock(&coder->coder_mutex);
		if (check_burnout(all, all->coders, left, get_time()))
		{
			pthread_mutex_unlock(&coder->coder_mutex);
			break ;
		}
		pthread_mutex_unlock(&coder->coder_mutex);
		ft_usleep(all->args->dongle_cooldown, coder);
	}
}