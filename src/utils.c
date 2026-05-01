/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 16:43:45 by opernod          #+#    #+#             */
/*   Updated: 2026/05/01 13:17:15 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	is_space(char c)
{
	return (c == ' ' || c == '\t' || c == '\n' || c == '\r');
}

long	ft_atol(const char *str)
{
	int		sign;
	int		i;
	long	nb;

	sign = 1;
	i = 0;
	nb = 0;
	if (!str)
		return (0);
	while (is_space(str[i]))
		i++;
	if (str[i] == '-' || str[i] == '+')
	{
		if (str[i++] == '-')
			sign = -1;
	}
	while (str[i] >= '0' && str[i] <= '9')
	{
		nb = nb * 10 + str[i++] - '0';
		if (nb > INT_MAX && sign == 1)
			return (LONG_MAX);
		if (nb > ((long long)INT_MAX + 1) && sign == -1)
			return (LONG_MIN);
	}
	return (sign * nb);
}
