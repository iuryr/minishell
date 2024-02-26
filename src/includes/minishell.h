/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: Juliany Bernardo <julberna@student.42sp    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/01/10 13:44:05 by julberna          #+#    #+#             */
/*   Updated: 2024/02/26 11:48:16 by iusantos         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include "../../libft/libft.h"
# include <stdio.h>
# include <errno.h>
# include <fcntl.h>
# include <signal.h>
# include <stddef.h>
# include <unistd.h>
# include <string.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <linux/limits.h>
# include <readline/history.h>
# include <readline/readline.h>

# define LIE 0
# define TRUTH 1

# define WORD 10
# define REDIRECT 11

# define REDOUT 20
# define APPEND 21
# define REDIN 22
# define HEREDOC 23

# define CMD 30
# define PIPELINE 31

# define HT_SIZE 10

typedef struct s_meta
{
	struct s_token	*tokens;
	struct s_ast	*ast;
	struct s_hash	**hash;
}				t_meta;

typedef struct s_hash
{
	char			*name;
	char			*value;
	struct s_hash	*next;
}				t_hash;

typedef struct s_token
{
	int				type;
	char			*literal;
	struct s_token	*next;
}				t_token;

typedef struct s_ast
{
	int				type;
	int				success;
	struct s_ast	*parent;
	struct s_ast	*left;
	struct s_ast	*right;
	struct s_cmd	*data;
}				t_ast;

typedef struct s_lexer
{
	char			*input;
	char			ch;
	unsigned int	pos;
	unsigned int	size;
	unsigned int	success;
	unsigned int	read_pos;
}				t_lexer;

typedef struct s_cmd
{
	char			*pathname;
	struct s_word	*word_list;
	struct s_redir	*redirects;
}				t_cmd;

typedef struct s_word
{
	char			*word;
	struct s_word	*next;
}				t_word;

typedef struct s_redir
{
	int				type;
	char			*filename;
	struct s_redir	*next;
}				t_redir;

typedef struct s_builtin
{
	char	*cmd_name;
	int		(*function)(t_meta *, t_word *);
}				t_builtin;

/* MAIN CALLS */
int				lexer(t_meta *meta);
int				parser(t_meta *meta);
void			executor(t_meta *meta);
void			set_meta(t_meta *meta, char **__environ);
void			finisher(t_meta meta, char *flag, int exit_code);

/* LEXER */
void			find_token(t_lexer *lex, t_token **tokens, int size);
void			replace_variable(char **input, t_hash **hash);
void			expand_variable(char **input, t_meta *meta);
void			set_lexer(t_lexer *lex, char *input);
void			read_char(t_lexer *lex);
char			*read_unquoted(t_lexer *lex);
char			*get_variable_name(char *literal);
char			*read_quoted(t_lexer *lex, char quote, int s_open, int d_open);

/* PARSER */
void			remove_quotes(t_token **tokens);
void			find_path(t_ast **ast, char **paths);
void			get_path(t_ast **ast, t_hash **hash);
void			remove_empty_tokens(t_token **tokens);
void			syntax_error(char *token, t_meta *meta);
void			set_cmd(t_ast **cmd_node, t_ast **parent);
void			set_pl(t_ast **pl, t_ast **parent, t_token **tk, t_meta *meta);
t_ast			*parse_cmd(t_token **tokens, t_ast *parent, t_meta *meta);
t_ast			*parse_pipeline(t_token **tokens, t_ast *parent, t_meta *meta);

/* EXECUTOR */
int				get_size(t_word *wl);
int				handle_exit_status(t_meta *meta);
int				run_builtin(t_meta *meta, t_word *wl);
void			run_pipeline(t_ast *ast, t_meta *meta);
void			run_executable(t_cmd *data, t_meta *meta);
void			run_simple_command(t_ast *cmd, t_meta *meta);
void			handle_null_pathname(char *cmd, t_meta *meta);
void			exec_forked_command(t_cmd *data, t_meta *meta);
void			exec_right(t_cmd *data, int pipe_fd[2], t_meta *meta);
void			upd_simple_exit_status(int exit_status, t_meta	*meta);
void			last_pipeline_cmd(t_ast *ast, int *pipe_fd, t_meta *meta);
void			middle_pipeline_cmd(t_ast *ast, int *pipe_fd, t_meta *meta);
void			first_pipeline_cmd(t_ast *ast, int pipe_fd[2], t_meta *meta);
void			exec_left(t_cmd *data, int in_fd, int pipe_fd[2], t_meta *meta);
char			**stringfy(t_word *word_list);

/* LIST HANDLERS */
void			sort_vars(t_word **vars, t_word *first, t_word *first_p);
void			append_redirect(t_redir *new_node, t_redir **redir_list);
void			new_token(t_token **tokens, int type, char *literal);
void			append_wle(t_word *new_node, t_word **word_list);
void			populate_sort_vars(t_hash **ht, t_word **vars);
t_word			*new_wle(char *string);
t_token			*tk_last(t_token *tokens);
t_redir			*new_redirect(t_token *tokens);

/* SIGNALS */
void			signal_handler(void);
void			sig_deal(int signo);

/* HASH TABLE */
char			*grab_value(char *name, t_hash **ht);
void			set_hashtable(char **env, t_hash **ht);
void			add_upd_hashtable(char *name, char *value, t_hash **ht);
t_hash			*create_kv_pair(char *name, char *value);
unsigned int	hash(char *name);

/* BUILTINS */
int				cd(t_meta *meta, t_word *wl);
int				pwd(t_meta *meta, t_word *wl);
int				env(t_meta *meta, t_word *wl);
int				echo(t_meta *meta, t_word *wl);
int				unset(t_meta *meta, t_word *wl);
int				export(t_meta *meta, t_word *wl);
int				ft_exit(t_meta *meta, t_word *wl);
void			print_export(t_word *vars, t_hash **ht);

/* CONDITIONALS */
int				is_operand(char ch);
int				sorted(t_word *vars);
int				is_builtin(char *cmd);
int				is_readonly(char *literal);
int				has_variable(char *literal);
int				valid_variable(char *literal);
int				has_other_pipes(t_token *tokens);
int				quote_open(char ch, char next, int s_open, int d_open);

/* FINISHERS */
void			safe_free(void *p);
void			close_all_fds(void);
void			free_ast(t_ast *ast);
void			free_cmd(t_cmd	*cmd);
void			free_hash(t_hash **ht);
void			free_ht_entry(t_hash *ht);
void			finish_lexer(t_lexer *lex);
void			free_tokens(t_token *tokens);
void			free_wordlist(t_word *word_list);
void			free_redirects(t_redir *redir_list);
void			free_str_array(char **array, int size);

#endif
