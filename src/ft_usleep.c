/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_usleep.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/24 12:45:00 by opernod           #+#    #+#             */
/*   Updated: 2026/04/24 17:49:49 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

void	ft_usleep(long time_in_ms, t_all *all)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < time_in_ms && check_running(all))
		usleep(50);
}
