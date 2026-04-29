/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parssing.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/29 15:03:39 by opernod           #+#    #+#             */
/*   Updated: 2026/04/29 15:03:41 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	parse_int(const char *str)
{
	long	res;
	int		i;

	res = 0;
	i = 0;
	if (!str || !str)
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

static int	check_args(t_args *args, char **argv)
{
	if (args->number_of_coders < 0 || args->time_to_burnout < 0
		|| args->time_to_compile < 0 || args->time_to_debug < 0
		|| args->time_to_refactor < 0
		|| args->number_of_compiles_required < 0
		|| args->dongle_cooldown < 0)
	{
		printf("Error: invalid arguments (positive integers <= INT_MAX)\n");
		return (1);
	}
	if (args->number_of_coders == 0 || args->time_to_burnout == 0
		|| args->number_of_compiles_required == 0)
	{
		printf("Error: number_of_coders, time_to_burnout and ");
		printf("number_of_compiles_required must be positive integers\n");
		return (1);
	}
	if (strcmp(argv[8], "fifo") != 0 && strcmp(argv[8], "edf") != 0)
	{
		printf("Error: scheduler must be 'fifo' or 'edf'\n");
		return (1);
	}
	return (0);
}

int	parssing(t_args *args, int argc, char **argv)
{
	if (argc != 9)
	{
		printf("Usage: ./codexion number_of_coders time_to_burnout ");
		printf("time_to_compile time_to_debug time_to_refactor ");
		printf("number_of_compiles_required dongle_cooldown scheduler\n");
		return (1);
	}
	args->number_of_coders = parse_int(argv[1]);
	args->time_to_burnout = parse_int(argv[2]);
	args->time_to_compile = parse_int(argv[3]);
	args->time_to_debug = parse_int(argv[4]);
	args->time_to_refactor = parse_int(argv[5]);
	args->number_of_compiles_required = parse_int(argv[6]);
	args->dongle_cooldown = parse_int(argv[7]);
	args->scheduler = argv[8];
	if (check_args(args, argv))
		return (1);
	return (0);
}

void	cleanup(t_args *args, t_all *a, t_coder *co, pthread_mutex_t *mut)
{
	int	i;

	i = -1;
	while (++i < args->number_of_coders)
	{
		if (co[i].thread_id)
			pthread_join(co[i].thread_id, NULL);
		pthread_mutex_destroy(&co[i].coder_mutex);
		pthread_mutex_destroy(&a->dongle_mutexes[i]);
	}
	if (a->dongle_mutexes)
		free(a->dongle_mutexes);
	pthread_mutex_destroy(mut);
	pthread_mutex_destroy(&a->run_mutex);
	free_all(args, a, co);
}
