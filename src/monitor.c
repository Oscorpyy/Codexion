/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 16:54:39 by opernod           #+#    #+#             */
/*   Updated: 2026/05/01 13:18:04 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	has_burnout(t_coder *coder, long current_time)
{
	long	last_compile_time;

	pthread_mutex_lock(&coder->mutex);
	last_compile_time = coder->last_compile_time;
	pthread_mutex_unlock(&coder->mutex);
	if (current_time - last_compile_time >= coder->data->args.time_burnout)
		return (1);
	return (0);
}

static int	handle_burnout(t_data *data, int i, long current_time,
		int nb_coders)
{
	if (has_burnout(&data->all_coders[i], current_time))
	{
		pthread_mutex_lock(&data->sim_mutex);
		data->simulation_running = 0;
		pthread_mutex_unlock(&data->sim_mutex);
		print_status(&data->all_coders[i], "has burned out", 1);
		i = -1;
		while (++i < nb_coders)
		{
			pthread_mutex_lock(&data->all_dongles[i].mutex);
			pthread_cond_broadcast(&data->all_dongles[i].cond);
			pthread_mutex_unlock(&data->all_dongles[i].mutex);
		}
		return (0);
	}
	return (1);
}

static int	enough_compiles(t_data *data)
{
	int	i;

	i = -1;
	while (++i < data->args.nb_coders)
	{
		pthread_mutex_lock(&data->all_coders[i].mutex);
		if (!(data->all_coders[i].nb_compiles
				>= data->args.nb_compiles_required))
		{
			pthread_mutex_unlock(&data->all_coders[i].mutex);
			return (0);
		}
		pthread_mutex_unlock(&data->all_coders[i].mutex);
	}
	pthread_mutex_lock(&data->sim_mutex);
	data->simulation_running = 0;
	pthread_mutex_unlock(&data->sim_mutex);
	i = -1;
	while (++i < data->args.nb_coders)
	{
		pthread_mutex_lock(&data->all_dongles[i].mutex);
		pthread_cond_broadcast(&data->all_dongles[i].cond);
		pthread_mutex_unlock(&data->all_dongles[i].mutex);
	}
	return (1);
}

static void	*unlock_mutex(pthread_mutex_t *mutex)
{
	pthread_mutex_unlock(mutex);
	return (NULL);
}

void	*monitor_routine(void *arg)
{
	t_data	*data;
	int		nb_coders;
	int		i;
	long	cur_time;

	data = (t_data *)arg;
	nb_coders = data->args.nb_coders;
	while (1)
	{
		i = -1;
		pthread_mutex_lock(&data->sim_mutex);
		if (!data->simulation_running)
			return (unlock_mutex(&data->sim_mutex));
		pthread_mutex_unlock(&data->sim_mutex);
		if (enough_compiles(data))
			return (NULL);
		cur_time = get_time_in_ms();
		while (++i < nb_coders)
		{
			if (!handle_burnout(data, i, cur_time, nb_coders))
				return (NULL);
		}
		ft_usleep(1, data);
	}
}
