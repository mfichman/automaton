#include <environment.hpp>
#include <interpreter.hpp>
#include <executor.hpp>
#include <rule.hpp>
#include <iostream>

int main(int argc, char** argv)
{
	Automaton::Environment env;
	Automaton::Interpreter interp(argv[1]);
	Automaton::Executor exec(interp);
	
	exec.load(argv[2]);
	exec.run();
	
	std::cout << "OK" << std::endl;
}
