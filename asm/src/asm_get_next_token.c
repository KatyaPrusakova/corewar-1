#include "asm.h"
#include <ctype.h>

t_token	asm_init_token(t_token_type type, char *value, size_t row, size_t col)
{
	t_token	token;

	token.type = type;
	token.value = value;
	token.file_row = row;
	token.file_col = col;
	return (token);
}

void	asm_lexer_skip_whitespace(t_lexer *lexer)
{
	while (lexer->current_char == ' ' || lexer->current_char == '\t')
		asm_lexer_advance(lexer);
}

void	asm_lexer_skip_comment(t_lexer *lexer)
{
	asm_lexer_advance(lexer);
	while (lexer->current_char != '\0' && lexer->current_char != '\n')
		asm_lexer_advance(lexer);
}

t_token	asm_get_eof_token(t_lexer *lexer)
{
	t_token	token;

	token = asm_init_token(EOF_TOKEN, NULL, lexer->file_row, lexer->file_col);
	return (token);
}

t_token	asm_get_error_token(t_lexer *lexer)
{
	t_token	token;

	token = asm_init_token(ERROR_TOKEN, NULL, lexer->file_row, lexer->file_col);
	return (token);
}

t_token	asm_get_int_token(t_lexer *lexer)
{
	t_token		token;
	const char	*token_start;
	size_t		token_len;

	token = asm_init_token(INTEGER_TOKEN, NULL, lexer->file_row, lexer->file_col);
	token_start = &lexer->input[lexer->current_pos];
	token_len = 0;
	if (lexer->current_char == '-')
	{
		asm_lexer_advance(lexer);
		token_len++;
	}
	while (isdigit(lexer->current_char))
	{
		asm_lexer_advance(lexer);
		token_len++;
	}
	token.value = strndup(token_start, token_len);
	return (token);
}

t_token	asm_get_id_token(t_lexer *lexer)
{
	t_token		token;
	const char	*token_start;
	size_t		token_len;

	token = asm_init_token(ID_TOKEN, NULL, lexer->file_row, lexer->file_col);
	token_start = &lexer->input[lexer->current_pos];
	token_len = 0;
	while (isalpha(lexer->current_char)
		|| isdigit(lexer->current_char)
		|| lexer->current_char == '_')
	{
		asm_lexer_advance(lexer);
		token_len++;
	}
	token.value = strndup(token_start, token_len);
	return (token);
}

t_token	asm_get_dot_token(t_lexer *lexer)
{
	t_token		token;
	const char	*token_start;
	size_t		token_len;

	token = asm_init_token(DOT_CMD_TOKEN, NULL, lexer->file_row, lexer->file_col);
	token_start = &lexer->input[lexer->current_pos];
	asm_lexer_advance(lexer);
	token_len = 1;
	while (isalpha(lexer->current_char)
		|| isdigit(lexer->current_char)
		|| lexer->current_char == '_')
	{
		token_len++;
		asm_lexer_advance(lexer);
	}
	token.value = strndup(token_start, token_len);
	return (token);
}

t_token	asm_get_string_token(t_lexer *lexer)
{
	t_token		token;
	const char	*token_start;
	size_t		token_len;

	token = asm_init_token(STRING_TOKEN, NULL, lexer->file_row, lexer->file_col);
	token_start = &lexer->input[lexer->current_pos];
	asm_lexer_advance(lexer);
	token_len = 1;
	while (lexer->current_char != '"' && lexer->current_char != '\0')
	{
		token_len++;
		asm_lexer_advance(lexer);
	}
	if (lexer->current_char != '"')
		token.type = ERROR_TOKEN;
	else
	{
		token_len++;
		asm_lexer_advance(lexer);
		token.value = strndup(token_start, token_len);
	}
	return (token);
}

t_token	asm_get_character_token(t_lexer *lexer)
{
	t_token	token;

	token = asm_init_token(ERROR_TOKEN, NULL, lexer->file_row, lexer->file_col);
	if (lexer->current_char == '\n' || lexer->current_char == '\r')
		token.type = NEWLINE_TOKEN;
	else if (lexer->current_char == SEPARATOR_CHAR)
		token.type = SEPARATOR_TOKEN;
	else if (lexer->current_char == LABEL_CHAR)
		token.type = LABEL_TOKEN;
	else if (lexer->current_char == DIRECT_CHAR)
		token.type = DIRECT_TOKEN;
	token.value = strndup(&lexer->input[lexer->current_pos], 1);
	asm_lexer_advance(lexer);
	return (token);
}

t_token	asm_get_next_token(t_lexer *lexer)
{
	while (lexer->current_char != '\0')
	{
		if (lexer->current_char == ' ' || lexer->current_char == '\t')
		{
			asm_lexer_skip_whitespace(lexer);
			continue ;
		}
		if (lexer->current_char == COMMENT_CHAR)
		{
			asm_lexer_skip_comment(lexer);
			continue ;
		}
		if (lexer->current_char == '\n' || lexer->current_char == '\r'
			|| lexer->current_char == SEPARATOR_CHAR
			|| lexer->current_char == LABEL_CHAR
			|| lexer->current_char == DIRECT_CHAR)
			return (asm_get_character_token(lexer));
		else if (lexer->current_char == '.')
			return (asm_get_dot_token(lexer));
		else if (lexer->current_char == '"')
			return (asm_get_string_token(lexer));
		else if (isdigit(lexer->current_char) || lexer->current_char == '-')
			return (asm_get_int_token(lexer));
		else if (isalpha(lexer->current_char))
			return (asm_get_id_token(lexer));
		else
			return (asm_get_error_token(lexer));
	}
	return (asm_get_eof_token(lexer));
}
