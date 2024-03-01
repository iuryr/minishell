/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirects.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: iusantos <iusantos@student.42sp.org.br>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/01 14:32:57 by iusantos          #+#    #+#             */
/*   Updated: 2024/03/01 17:07:37 by iusantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	simple_command_redirects(t_redir *rl, t_meta *meta)
{
	int	process_next_redir;

	while (rl != NULL)
	{
		if (rl->type == REDIN)
			process_next_redir = red_input(rl);
		else if (rl->type == APPEND)
			process_next_redir = red_append(rl);
		else if (rl->type == REDOUT)
			process_next_redir = red_output(rl);
		else
			process_next_redir = red_heredoc(rl);
		if (process_next_redir == LIE)
		{
			add_upd_hashtable("?", "1", meta->hash);
			return (LIE);
		}
		else
			rl = rl->next;
	}
	add_upd_hashtable("?", "0", meta->hash);
	return (TRUTH);
}

int	red_input(t_redir *r)
{
	int	fd;

	if (access(r->filename, F_OK) == 0)
	{
		if(access(r->filename, R_OK) != 0)
		{
			print_np_error_msg(r->filename);
			return (LIE);
		}
		fd = open(r->filename, O_RDONLY);
		dup2(fd, STDIN_FILENO);
		close(fd);
		return (TRUTH);
	}
	else
	{
		print_nsf_error_msg(r->filename);
		return (LIE);
	}
}

int	red_output(t_redir *r)
{
	int	fd;

	if (access(r->filename, F_OK) == 0)
	{
		if(access(r->filename, W_OK) != 0)
		{
			print_np_error_msg(r->filename);
			return (LIE);
		}
	}
	fd = open(r->filename, O_WRONLY|O_TRUNC|O_CREAT, 0644);
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (TRUTH);
}

int	red_heredoc(t_redir *r)
{
	r = 0;
	return 0;
}

int	red_append(t_redir *r)
{
	int	fd;

	if (access(r->filename, F_OK) == 0)
	{
		if(access(r->filename, W_OK) != 0)
		{
			print_np_error_msg(r->filename);
			return (LIE);
		}
	}
	fd = open(r->filename, O_WRONLY|O_APPEND|O_CREAT, 0644);
	dup2(fd, STDOUT_FILENO);
	close(fd);
	return (TRUTH);
}

void	print_nsf_error_msg(char *filename)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(filename, 2);
	ft_putstr_fd(": no such file or directory\n", 2);
}

void	print_np_error_msg(char *filename)
{
	ft_putstr_fd("minishell: ", 2);
	ft_putstr_fd(filename, 2);
	ft_putstr_fd(": permission denied\n", 2);
}
