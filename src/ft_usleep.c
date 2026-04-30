/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_usleep.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 18:09:54 by opernod           #+#    #+#             */
/*   Updated: 2026/04/30 12:39:06 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"

void	ft_usleep(long time_in_ms, t_coder *coder)
{
	long	start;

	start = get_time();
	while ((get_time() - start) < time_in_ms && check_running(coder->all))
	{
		usleep(500);
	}
}
