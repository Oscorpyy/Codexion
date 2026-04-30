/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_usleep.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 18:09:54 by opernod           #+#    #+#             */
/*   Updated: 2026/04/30 16:06:14 by opernod          ###   ########lyon.fr   */
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

void	set_order(t_coder *c, int l, int r, t_order *o)
{
	if (l < r)
	{
		o->first = l;
		o->f_flag = &c->has_left;
		o->second = r;
		o->s_flag = &c->has_right;
	}
	else
	{
		o->first = r;
		o->f_flag = &c->has_right;
		o->second = l;
		o->s_flag = &c->has_left;
	}
}
