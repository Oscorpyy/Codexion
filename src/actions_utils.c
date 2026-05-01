/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 11:47:16 by opernod           #+#    #+#             */
/*   Updated: 2026/05/01 13:45:24 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	add_fifo(t_dongle *dongle, t_coder *coder)
{
	if (dongle->nb_coders_waiting == 0)
		dongle->coders[0] = coder;
	else
	{
		if (dongle->coders[0] == coder)
			return (0);
		dongle->coders[1] = coder;
	}
	return (1);
}

static int	add_edf(t_dongle *dongle, t_coder *coder)
{
	t_coder	*tmp;

	if (dongle->nb_coders_waiting == 0)
		dongle->coders[0] = coder;
	else
	{
		if (dongle->coders[0] == coder)
			return (0);
		tmp = dongle->coders[0];
		if (coder->id < tmp->id)
			lock_two_mutexes(coder, tmp);
		else
			lock_two_mutexes(tmp, coder);
		if (coder->last_compile_time < tmp->last_compile_time)
		{
			dongle->coders[1] = tmp;
			dongle->coders[0] = coder;
		}
		else
			dongle->coders[1] = coder;
		pthread_mutex_unlock(&tmp->mutex);
		pthread_mutex_unlock(&coder->mutex);
	}
	return (1);
}

void	try_add_to_queue(t_dongle *dongle, t_coder *coder)
{
	pthread_mutex_lock(&dongle->mutex);
	if (dongle->nb_coders_waiting == 2)
	{
		pthread_mutex_unlock(&dongle->mutex);
		return ;
	}
	if (coder->data->args.scheduler == FIFO)
	{
		if (!add_fifo(dongle, coder))
		{
			pthread_mutex_unlock(&dongle->mutex);
			return ;
		}
	}
	else
	{
		if (!add_edf(dongle, coder))
		{
			pthread_mutex_unlock(&dongle->mutex);
			return ;
		}
	}
	dongle->nb_coders_waiting++;
	pthread_mutex_unlock(&dongle->mutex);
}

int	is_cooldown_active(t_dongle *dongle, t_args args)
{
	long	current_time;
	long	time_since_usage;

	current_time = get_time_in_ms();
	time_since_usage = current_time - dongle->last_used;
	if (time_since_usage < args.dongle_cooldown)
		return (1);
	return (0);
}

void	pop_queue(t_dongle *dongle)
{
	if (dongle->nb_coders_waiting == 2)
		dongle->coders[0] = dongle->coders[1];
	else
		dongle->coders[1] = NULL;
	dongle->nb_coders_waiting--;
}
