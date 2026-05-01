/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 11:11:16 by opernod           #+#    #+#             */
/*   Updated: 2026/05/01 13:18:17 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static void	sleep_time_left(t_dongle *dongle, t_data *data)
{
	long	time_left;

	time_left = data->args.dongle_cooldown - (get_time_in_ms()
			- dongle->last_used);
	pthread_mutex_unlock(&dongle->mutex);
	ft_usleep(time_left, data);
	pthread_mutex_lock(&dongle->mutex);
}

static int	take_dongle(t_dongle *dongle, t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	while (1)
	{
		pthread_mutex_lock(&coder->data->sim_mutex);
		if (!coder->data->simulation_running)
		{
			pthread_mutex_unlock(&coder->data->sim_mutex);
			pthread_mutex_unlock(&dongle->mutex);
			return (0);
		}
		pthread_mutex_unlock(&coder->data->sim_mutex);
		if (dongle->is_currently_used || dongle->coders[0] != coder)
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
		else if (is_cooldown_active(dongle, coder->data->args))
			sleep_time_left(dongle, coder->data);
		else
			break ;
	}
	if (dongle->coders[0] == coder)
		pop_queue(dongle);
	dongle->is_currently_used = 1;
	pthread_mutex_unlock(&dongle->mutex);
	return (1);
}

static void	release_single_dongle(t_dongle *dongle)
{
	pthread_mutex_lock(&dongle->mutex);
	dongle->is_currently_used = 0;
	dongle->last_used = get_time_in_ms();
	pthread_cond_broadcast(&dongle->cond);
	pthread_mutex_unlock(&dongle->mutex);
}

int	take_dongles(t_coder *coder, t_args args)
{
	if (get_time_in_ms() - coder->last_compile_time >= args.time_burnout)
	{
		ft_usleep(1, coder->data);
		return (-1);
	}
	try_add_to_queue(coder->first_dongle, coder);
	if (!take_dongle(coder->first_dongle, coder))
		return (-1);
	print_status(coder, "has taken a dongle", 0);
	if (coder->data->args.nb_coders == 1)
	{
		ft_usleep(coder->data->args.time_burnout, coder->data);
		return (0);
	}
	try_add_to_queue(coder->second_dongle, coder);
	if (!take_dongle(coder->second_dongle, coder))
	{
		release_single_dongle(coder->first_dongle);
		return (-1);
	}
	print_status(coder, "has taken a dongle", 0);
	return (1);
}

void	release_dongles(t_coder *coder)
{
	release_single_dongle(coder->first_dongle);
	release_single_dongle(coder->second_dongle);
}
