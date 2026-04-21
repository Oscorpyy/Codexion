#define _XOPEN_SOURCE 500
#include "../includes/codexion.h"
#include <sys/time.h>
#include <unistd.h>

long get_time(void);
int check_running(t_all *all);
void print_state(t_coder *coder, char *state);

void	acquire_dongles_fifo(t_all *all, t_coder *coder)
{
	while (check_running(all))
	{
		print_state(coder, "has taken a dongle");
		print_state(coder, "has taken a dongle");
		print_state(coder, "is compiling");

		pthread_mutex_lock(&coder->coder_mutex);
		coder->last_compile_time = get_time();
		pthread_mutex_unlock(&coder->coder_mutex);

		usleep(all->args->time_to_compile * 1000);

		pthread_mutex_lock(&coder->coder_mutex);
		coder->compiles_done++;
		pthread_mutex_unlock(&coder->coder_mutex);

		if (all->args->number_of_compiles_required != -1 && coder->compiles_done >= all->args->number_of_compiles_required)
			break;

		print_state(coder, "is debugging");
		usleep(all->args->time_to_debug * 1000);

		print_state(coder, "is refactoring");
		usleep(all->args->time_to_refactor * 1000);
	}
}
