/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 11:11:19 by opernod           #+#    #+#             */
/*   Updated: 2026/05/01 13:43:55 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

void	print_status(t_coder *coder, char *msg, int force)
{
	long	time;
	int		can_print;

	pthread_mutex_lock(&coder->data->print_mutex);
	pthread_mutex_lock(&coder->data->sim_mutex);
	can_print = coder->data->simulation_running || force;
	pthread_mutex_unlock(&coder->data->sim_mutex);
	if (can_print)
	{
		time = get_time_in_ms() - coder->data->time_at_start;
		printf("%ld %d %s\n", time, coder->id, msg);
	}
	pthread_mutex_unlock(&coder->data->print_mutex);
}

static void	wait_for_start(t_coder *coder)
{
	int	can_start;

	can_start = 0;
	pthread_mutex_lock(&coder->data->sim_mutex);
	coder->data->started_threads++;
	pthread_mutex_unlock(&coder->data->sim_mutex);
	while (1)
	{
		pthread_mutex_lock(&coder->data->sim_mutex);
		can_start = coder->data->started_threads == coder->data->args.nb_coders;
		if (!coder->data->simulation_running)
		{
			pthread_mutex_unlock(&coder->data->sim_mutex);
			return ;
		}
		pthread_mutex_unlock(&coder->data->sim_mutex);
		if (!can_start)
			usleep(80);
		else
			break ;
	}
}

static void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = (t_coder *)arg;
	wait_for_start(coder);
	if (coder->id % 2 == 0)
		ft_usleep(1, coder->data);
	while (1)
	{
		if (take_dongles(coder, coder->data->args) <= 0)
			return (NULL);
		print_status(coder, "is compiling", 0);
		pthread_mutex_lock(&coder->mutex);
		coder->last_compile_time = get_time_in_ms();
		coder->nb_compiles++;
		pthread_mutex_unlock(&coder->mutex);
		ft_usleep(coder->data->args.time_compile, coder->data);
		release_dongles(coder);
		print_status(coder, "is debugging", 0);
		ft_usleep(coder->data->args.time_debug, coder->data);
		print_status(coder, "is refactoring", 0);
		ft_usleep(coder->data->args.time_refactor, coder->data);
	}
	return (NULL);
}

void	join_threads(t_data *data, int count)
{
	int	i;

	i = 0;
	while (i < count)
	{
		pthread_join(data->all_coders[i].thread, NULL);
		i++;
	}
	pthread_join(data->monitor_thread, NULL);
}

int	start_threads(t_data *data)
{
	int	i;

	i = -1;
	data->time_at_start = get_time_in_ms();
	while (++i < data->args.nb_coders)
		data->all_coders[i].last_compile_time = data->time_at_start;
	i = 0;
	data->started_threads = 0;
	if (pthread_create(&data->monitor_thread, NULL, &monitor_routine,
			data) != 0)
		return (0);
	while (i < data->args.nb_coders)
	{
		if (pthread_create(&data->all_coders[i].thread, NULL, &coder_routine,
				&data->all_coders[i]) != 0)
		{
			pthread_mutex_lock(&data->sim_mutex);
			data->simulation_running = 0;
			pthread_mutex_unlock(&data->sim_mutex);
			join_threads(data, i);
			return (0);
		}
		i++;
	}
	return (1);
}
