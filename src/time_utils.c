/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/19 11:12:06 by opernod           #+#    #+#             */
/*   Updated: 2026/05/01 13:17:56 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

long	get_time_in_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, (void *)0);
	return ((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

void	ft_usleep(long time_in_ms, t_data *data)
{
	long	start_time;

	if (time_in_ms <= 0)
	{
		usleep(50);
		return ;
	}
	start_time = get_time_in_ms();
	while ((get_time_in_ms() - start_time) < time_in_ms)
	{
		pthread_mutex_lock(&data->sim_mutex);
		if (!data->simulation_running)
		{
			pthread_mutex_unlock(&data->sim_mutex);
			break ;
		}
		pthread_mutex_unlock(&data->sim_mutex);
		usleep(100);
	}
}
