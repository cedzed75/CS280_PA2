/* Definitions and some functions implementations
 * parse.cpp to be completed
 * Programming Assignment 2
 * Spring 2022
*/

#include "parse.h"


map<string, bool> defVar;
map<string, Token> SymTable;

namespace Parser {
	bool pushed_back = false;
	LexItem	pushed_token;

	static LexItem GetNextToken(istream& in, int& line) {
		if (pushed_back) {
			pushed_back = false;
			return pushed_token;
		}
		return getNextToken(in, line);
	}

	static void PushBackToken(LexItem& t) {
		if (pushed_back) {
			abort();
		}
		pushed_back = true;
		pushed_token = t;
	}

}

static int error_count = 0;

int ErrCount()
{
	return error_count;
}

void ParseError(int line, string msg)
{
	++error_count;
	cout << line << ": " << msg << endl;
}


bool Prog(istream& in, int& line)
{
	bool status = false;

	LexItem t = Parser::GetNextToken(in, line);
	if (t.GetToken() == PROGRAM)
	{
		t = Parser::GetNextToken(in, line);
		if (t == IDENT)
		{
			t = Parser::GetNextToken(in, line);
			if (t == SEMICOL)
			{
				status = DeclBlock(in, line);
				if (status)
				{
					status = ProgBody(in, line);
					if (status)
					{
						return true;
					}
					else
					{
						return false;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
	return false;
}

bool DeclBlock(istream& in, int& line)
{
	bool status = false;
	bool looper = true;

	LexItem t;
	status = Var(in, line);

	if (status)
	{
		while (looper)
		{
			status = DeclStmt(in, line);
			if (status)
			{
				t = Parser::GetNextToken(in, line);
				if (t.GetToken() != SEMICOL)
				{
					return false;
				}
				else
				{
					t = Parser::GetNextToken(in, line);
					if (t.GetToken() == BEGIN)
					{
						Parser::PushBackToken(t);
						return true;
					}
					Parser::PushBackToken(t);
				}
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return false;
}

bool DeclStmt(istream& in, int& line)
{
	return false;
}

bool ProgBody(istream& in, int& line)
{
	bool status = false;
	bool looper = true;
	LexItem t = Parser::GetNextToken(in, line);
	
	if (t.GetToken() == BEGIN)
	{
		while (looper)
		{
			status = Stmt(in, line);
			if (status)
			{
				t = Parser::GetNextToken(in, line);
				if (t.GetToken() != SEMICOL)
				{
					return false;
				}
				else
				{
					//Questionable, does this work?
					t = Parser::GetNextToken(in, line);
					if (t.GetToken() == END)
					{
						return true;
					}
					else
					{
						Parser::PushBackToken(t);
						looper = true;
					}
				}
			}
			else
			{
				return false;
			}
		}
	}
	else
	{
		return false;
	}
	return false;
}


//Stmt is either a WriteLnStmt, ForepeatStmt, IfStmt, or AssigStmt
//Stmt = AssigStmt | IfStmt | WriteStmt | ForStmt 
bool Stmt(istream& in, int& line) {
	bool status;
	//cout << "in ContrlStmt" << endl;
	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken()) {

	case WRITELN:
		status = WriteLnStmt(in, line);
		//cout << "After WriteStmet status: " << (status? true:false) <<endl;
		break;

	case IF:
		status = IfStmt(in, line);
		break;

	case IDENT:
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);

		break;

	case FOR:
		status = ForStmt(in, line);

		break;


	default:
		Parser::PushBackToken(t);
		return false;
	}

	return status;
}//End of Stmt


//WriteStmt:= wi, ExpreList 
bool WriteLnStmt(istream& in, int& line) {
	LexItem t;
	//cout << "in WriteStmt" << endl;

	t = Parser::GetNextToken(in, line);
	if (t != LPAREN) {

		ParseError(line, "Missing Left Parenthesis");
		return false;
	}

	bool ex = ExprList(in, line);

	if (!ex) {
		ParseError(line, "Missing expression after WriteLn");
		return false;
	}

	t = Parser::GetNextToken(in, line);
	if (t != RPAREN) {

		ParseError(line, "Missing Right Parenthesis");
		return false;
	}
	//Evaluate: print out the list of expressions values

	return ex;
}

bool IfStmt(istream& in, int& line)
{
	bool status = false;
	LexItem t = Parser::GetNextToken(in, line);

	if (t.GetToken() == IF)
	{
		status = LogicExpr(in, line);
		if (status)
		{
			t = Parser::GetNextToken(in, line);
			if (t.GetToken() == THEN)
			{
				status = Stmt(in, line);
				if (status)
				{
					t = Parser::GetNextToken(in, line);

					if (t == ELSE)
					{
						status = Stmt(in, line);
						if (status)
						{
							return true;
						}
						else
						{
							return false;
						}
					}
					else 
					{
						Parser::PushBackToken(t);
						return true;
					}
				}
				else
				{
					return false;
				}
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	status = Var(in, line);

	return false;
}

bool ForStmt(istream& in, int& line)
{
	return false;
}


bool AssignStmt(istream& in, int& line)
{
	bool status = false;
	status = Var(in, line);

	if (status)
	{
		LexItem t = Parser::GetNextToken(in, line);
		if (t.GetToken() == ASSOP)
		{
			status = Expr(in, line);
			if (status)
			{
				return true;
			}
			else
			{
				//Assignment statement not formed correctly
				return false;
			}
		}
		else
			return false;
	}
	else
	{
		return false;
	}
	return false;
}

//ExprList:= Expr {,Expr}
bool ExprList(istream& in, int& line) {
	bool status = false;
	//cout << "in ExprList and before calling Expr" << endl;
	status = Expr(in, line);
	if (!status) {
		ParseError(line, "Missing Expression");
		return false;
	}

	LexItem tok = Parser::GetNextToken(in, line);

	if (tok == COMMA) {
		//cout << "before calling ExprList" << endl;
		status = ExprList(in, line);
		//cout << "after calling ExprList" << endl;
	}
	else if (tok.GetToken() == ERR) {
		ParseError(line, "Unrecognized Input Pattern");
		cout << "(" << tok.GetLexeme() << ")" << endl;
		return false;
	}
	else {
		Parser::PushBackToken(tok);
		return true;
	}
	return status;
}

//Apparently in the slides
bool Expr(istream& in, int& line)
{
	bool status;

	status = Term(in, line);

	if (status)
	{
		LexItem t = Parser::GetNextToken(in, line);
		if (t.GetToken() == PLUS || t.GetToken() == MINUS)
		{
			status = Term(in, line);
			if (status)
			{
				return true;
			}
			else
			{
				//Term not formed correctly
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}

	return false;	
}

//TODO: Rework for repeatability
//Use while loop?
bool Term(istream& in, int& line)
{
	bool status;
	status = SFactor(in, line);
	
	if (status)
	{
		LexItem t = Parser::GetNextToken(in, line);
		if (t.GetToken() == MULT || t.GetToken() == DIV)
		{
			status = SFactor(in, line);
			if (status)
			{
				return true;
			}
			else
			{
				//SFACTOR not formed correctly
				return false;
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		return false;
	}

	return false;
}

//TODO: Repeat for Readability
//Use while loop?
bool SFactor(istream& in, int& line)
{
	bool status;

	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken())
	{
		case PLUS:
			status = Factor(in, line, 1);
			if (status)
			{
				return true;
			}
			else
			{
				return false;
			}
			break;

		case MINUS:
			status = Factor(in, line, -1);
			if (status)
				return true;
			else
				return false;
			break;

		default:
			status = Factor(in, line, 0);
			if (status)
				return true;
			else
				return false;
			
			break;
	}
	return false;
}

bool LogicExpr(istream& in, int& line)
{
	bool status = false;
	LexItem t = Parser::GetNextToken(in, line);

	status = Expr(in, line);

	if (status)
	{
		LexItem q = Parser::GetNextToken(in, line);
		if (q.GetToken() == LTHAN || q.GetToken() == EQUAL || q.GetToken() == GTHAN)
		{
			//Following Logical expression formatting;
			bool log = Expr(in, line);
			if (log)
			{
				return true;
			}
		}
		else
		{
			//Broke Logical Expression rule
			//ERROR MESSAGE
			return false;
		}
	}
	else
	{
		//Logical Expression does not contain expression
		return false;
	}
	return false;
}

bool Var(istream& in, int& line)
{
	bool status = false;

	LexItem t = Parser::GetNextToken(in, line);

	if (t.GetToken() == IDENT)
	{
		//Success State
		return true;
	}
	else
	{
		//Fail?
		//Print Error
		return false;
	}
}

bool Factor(istream& in, int& line, int sign)
{
	bool status;
	//cout << "in Factor" << endl;
	LexItem t = Parser::GetNextToken(in, line);

	switch (t.GetToken()) {

	case IDENT:
		return true;
		//return true;
		//cout << "After WriteStmet status: " << (status? true:false) <<endl;
		break;

	case ICONST:
		status = IfStmt(in, line);
		//return true;
		break;

	case RCONST:
		//return true;
		Parser::PushBackToken(t);
		status = AssignStmt(in, line);

		break;

	case SCONST:
		return true;
		status = ForStmt(in, line);

		break;


	default:
		status = Expr(in, line);
		if (!status)
		{
			//Spit out some error?
			return false;
		}
		return true;
	}
	return false;
}


