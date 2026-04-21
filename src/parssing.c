/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parssing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/08 12:59:29 by opernod           #+#    #+#             */
/*   Updated: 2026/04/21 16:13:05 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	parse_int(const char *str)
{
	long	res;
	int		i;

	res = 0;
	i = 0;
	if (!str || !str[0])
		return (-1);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (-1);
		res = res * 10 + (str[i] - '0');
		if (res > 2147483647)
			return (-1);
		i++;
	}
	return ((int)res);
}

int	parssing(t_args *args, int argc, char **argv)
{
	if (argc != 9)
	{
		printf("Usage: ./codexion number_of_coders time_to_burnout time_to_");
		printf("compile time_to_debug time_to_refactor number_of_compiles_");
		printf("required dongle_cooldown scheduler\n");
		return (1);
	}
	args->number_of_coders = parse_int(argv[1]);
	args->time_to_burnout = parse_int(argv[2]);
	args->time_to_compile = parse_int(argv[3]);
	args->time_to_debug = parse_int(argv[4]);
	args->time_to_refactor = parse_int(argv[5]);
	args->number_of_compiles_required = parse_int(argv[6]);
	args->dongle_cooldown = parse_int(argv[7]);
	if (args->number_of_coders < 0 || args->time_to_burnout < 0 || args->time_to_compile < 0
		|| args->time_to_debug < 0 || args->time_to_refactor < 0
		|| args->number_of_compiles_required <= 0 || args->dongle_cooldown < 0)
	{
		printf("Error: invalid arguments (must be positive integers <= INT_MAX)\n");
		return (1);
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
	{
		printf("Error: scheduler must be 'fifo' or 'edf'\n");
		return (1);
	}
	args->scheduler = argv[8];
	return (0);
}
