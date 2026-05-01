/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_data.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 11:11:42 by opernod           #+#    #+#             */
/*   Updated: 2026/05/01 13:18:10 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	free_data(t_data *data, int i)
{
	while (i-- >= 0)
	{
		pthread_mutex_destroy(&data->all_dongles[i].mutex);
		pthread_cond_destroy(&data->all_dongles[i].cond);
		pthread_mutex_destroy(&data->all_coders[i].mutex);
	}
	if (data->all_dongles)
		free(data->all_dongles);
	if (data->all_coders)
		free(data->all_coders);
	return (0);
}

static void	set_two_dongles(t_data *data, int i, int first_dongle,
		int second_dongle)
{
	data->all_coders[i].first_dongle = &data->all_dongles[first_dongle];
	data->all_coders[i].second_dongle = &data->all_dongles[second_dongle];
}

static int	init_coder(t_data *data, int i)
{
	int	left_dongle;
	int	right_dongle;

	if (pthread_mutex_init(&data->all_coders[i].mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->all_dongles[i].mutex);
		pthread_cond_destroy(&data->all_dongles[i].cond);
		return (0);
	}
	data->all_coders[i].data = data;
	data->all_coders[i].id = i + 1;
	data->all_coders[i].nb_compiles = 0;
	left_dongle = i;
	right_dongle = (i + 1) % data->args.nb_coders;
	if (data->args.nb_coders == 1)
	{
		data->all_coders[i].first_dongle = &data->all_dongles[0];
		data->all_coders[i].second_dongle = NULL;
		return (1);
	}
	if (left_dongle < right_dongle)
		set_two_dongles(data, i, left_dongle, right_dongle);
	else
		set_two_dongles(data, i, right_dongle, left_dongle);
	return (1);
}

static int	init_dongle(t_data *data, int i)
{
	data->all_dongles[i].is_currently_used = 0;
	if (pthread_mutex_init(&data->all_dongles[i].mutex, NULL) != 0)
	{
		while (i-- >= 0)
		{
			pthread_mutex_destroy(&data->all_dongles[i].mutex);
			pthread_cond_destroy(&data->all_dongles[i].cond);
		}
		return (0);
	}
	if (pthread_cond_init(&data->all_dongles[i].cond, NULL) != 0)
	{
		pthread_mutex_destroy(&data->all_dongles[i].mutex);
		while (i-- >= 0)
		{
			pthread_mutex_destroy(&data->all_dongles[i].mutex);
			pthread_cond_destroy(&data->all_dongles[i].cond);
		}
		return (0);
	}
	data->all_dongles[i].last_used = 0;
	data->all_dongles[i].nb_coders_waiting = 0;
	return (1);
}

int	init_data(t_data *data)
{
	int	i;

	i = -1;
	if (pthread_mutex_init(&data->sim_mutex, NULL) != 0)
		return (0);
	if (pthread_mutex_init(&data->print_mutex, NULL) != 0)
	{
		pthread_mutex_destroy(&data->sim_mutex);
		return (0);
	}
	data->time_at_start = get_time_in_ms();
	data->all_dongles = malloc(sizeof(t_dongle) * data->args.nb_coders);
	if (!data->all_dongles)
		return (0);
	data->all_coders = malloc(sizeof(t_coder) * data->args.nb_coders);
	if (!data->all_coders)
		return (free_data(data, i));
	while (++i < data->args.nb_coders)
	{
		if (!init_dongle(data, i) || !init_coder(data, i))
			return (free_data(data, i));
	}
	data->simulation_running = 1;
	return (1);
}
