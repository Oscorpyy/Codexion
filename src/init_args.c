/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_args.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: opernod <opernod@student.42lyon.fr>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/01/16 13:31:34 by opernod           #+#    #+#             */
/*   Updated: 2026/05/01 13:20:50 by opernod          ###   ########lyon.fr   */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/codexion.h"

static int	only_digits(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}

static void	set_arg(int arg_index, long nb, t_args *args)
{
	if (arg_index == 1)
		args->nb_coders = (int)nb;
	else if (arg_index == 2)
		args->time_burnout = (int)nb;
	else if (arg_index == 3)
		args->time_compile = (int)nb;
	else if (arg_index == 4)
		args->time_debug = (int)nb;
	else if (arg_index == 5)
		args->time_refactor = (int)nb;
	else if (arg_index == 6)
		args->nb_compiles_required = (int)nb;
	else if (arg_index == 7)
		args->dongle_cooldown = (int)nb;
}

static int	check_arg(char *arg, int arg_index, t_args *args)
{
	long	nb;

	if (arg_index == 8)
	{
		if (strcmp(arg, "fifo") == 0)
		{
			args->scheduler = FIFO;
			return (1);
		}
		else if (strcmp(arg, "edf") == 0)
		{
			args->scheduler = EDF;
			return (1);
		}
		else
			return (0);
	}
	else
	{
		nb = ft_atol(arg);
		if (!only_digits(arg) || nb > INT_MAX)
			return (0);
		set_arg(arg_index, nb, args);
	}
	return (1);
}

int	init_args(char **argv, t_args *args)
{
	int	i;

	i = 1;
	while (argv[i])
	{
		if (!check_arg(argv[i], i, args))
			return (0);
		i++;
	}
	return (1);
}
