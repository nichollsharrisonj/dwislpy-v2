#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <utility>
#include <functional>
#include <iostream>
#include <exception>
#include <algorithm>
#include <sstream>

#include "dwislpy-ast.hh"
#include "dwislpy-util.hh"

//
// predicate function for Valu
//
bool predicate(Valu e) {
    if (std::holds_alternative<int>(e)) {
        int i = std::get<int>(e);
        return (bool)i;
    }
    if (std::holds_alternative<bool>(e)) {
        bool b = std::get<bool>(e);
        return b;
    }
    if (std::holds_alternative<std::string>(e)) {
        std::string s = std::get<std::string>(e);
        return !(s.compare("") == 0);
    }
    return false;
}

//
// dwislpy-ast.cc
//
// Below are the implementations of methods for AST nodes. They are organized
// into groups. The first group represents the DWISLPY interpreter by giving
// the code for 
//
//    Prgm::run, Blck::exec, Stmt::exec, Expn::eval
//
// The second group AST::output and AST::dump performs pretty printing
// of SLPY code and also the output of the AST, resp.
//

//
// to_string
//
// Helper function that converts a DwiSlpy value into a string.
// This is meant to be used by `print` and also `str`.
// 
std::string to_string(Valu v) {
    if (std::holds_alternative<int>(v)) {
        return std::to_string(std::get<int>(v));
    } else if (std::holds_alternative<std::string>(v)) {
        return std::get<std::string>(v);
    } else if (std::holds_alternative<bool>(v)) {
        if (std::get<bool>(v)) {
            return "True";
        } else {
            return "False";
        }
    } else if (std::holds_alternative<none>(v)) {
        return "None";
    } else {
        return "<unknown>";
    }
}

//
// to_repr
//
// Helper function that converts a DwiSlpy value into a string.
// This is meant to mimic Python's `repr` function in that it gives
// the source code string for the value. Used by routines that dump
// a literal value.
//
std::string to_repr(Valu v) {
    if (std::holds_alternative<std::string>(v)) {
        //
        // Strings have to be converted to show their quotes and also
        // to have the unprintable chatacters given as \escape sequences.
        //
        return "\"" + re_escape(std::get<std::string>(v)) + "\"";
    } else {
        //
        // The other types aren't special. (This will have to change when
        // we have value types that aren't "ground" types e.g. list types.)
        //
        return to_string(v);
    }
}



// * * * * *
// The DWISLPY interpreter
//

//
// Prgm::run, Blck::exec, Stmt::exec
//
//  - execute DWISLPY statements, changing the runtime context mapping
//    variables to their current values.
//

void Prgm::run(void) const {
    Ctxt main_ctxt { };
    main->exec(defs,main_ctxt);
}

std::optional<Valu> Nest::exec(const Defs& defs, Ctxt& ctxt) const {
    std::optional<Valu> rv = blck->exec(defs,ctxt);
    if (rv.has_value()) {
        return rv;
    } 
    return std::nullopt;
}

std::optional<Valu> Blck::exec(const Defs& defs, Ctxt& ctxt) const {
    for (Stmt_ptr s : stmts) {
        std::optional<Valu> rv = s->exec(defs,ctxt);
        if (rv.has_value()) {
            return rv;
        } 
    }
    return std::nullopt;
}

std::optional<Valu> Asgn::exec(const Defs& defs,
                               Ctxt& ctxt) const {
    ctxt[name] = expn->eval(defs,ctxt);
    return std::nullopt;
}

std::optional<Valu> PlEq::exec(const Defs& defs,
                               Ctxt& ctxt) const {
    // from Lkup
    if (ctxt.count(name) <= 0) {
        std::string msg = "Run-time error: variable '" + name +"'";
        msg += "not defined.";
        throw DwislpyError { where(), msg };
    }

    Valu n = ctxt.at(name);
    Valu e = expn->eval(defs,ctxt);
    
    if (std::holds_alternative<int>(e) &&
        std::holds_alternative<int>(n)) {
        int in = std::get<int>(n);
        int ie = std::get<int>(e);
        ctxt[name] = Valu {in + ie};
    } else if (std::holds_alternative<std::string>(e) &&
               std::holds_alternative<std::string>(n)) {
        std::string sn = std::get<std::string>(n);
        std::string se = std::get<std::string>(e);
        ctxt[name] = Valu {sn + se};
    } else {
        std::string msg = "Run-time error: wrong operand type for plus equals.";
        throw DwislpyError { where(), msg };
    }        
    return std::nullopt;
}

std::optional<Valu> MiEq::exec(const Defs& defs,
                               Ctxt& ctxt) const {
    // from Lkup
    if (ctxt.count(name) <= 0) {
        std::string msg = "Run-time error: variable '" + name +"'";
        msg += "not defined.";
        throw DwislpyError { where(), msg };
    }

    Valu n = ctxt.at(name);
    Valu e = expn->eval(defs,ctxt);
    
    if (std::holds_alternative<int>(e) &&
        std::holds_alternative<int>(n)) {
        int in = std::get<int>(n);
        int ie = std::get<int>(e);
        ctxt[name] = Valu {in - ie};
    } else {
        std::string msg = "Run-time error: wrong operand type for minus equals.";
        throw DwislpyError { where(), msg };
    }        
    return std::nullopt;
}

std::optional<Valu> Pass::exec([[maybe_unused]] const Defs& defs,
                               [[maybe_unused]] Ctxt& ctxt) const {
    // does nothing!
    return std::nullopt;
}
  
std::optional<Valu> Prnt::exec(const Defs& defs, Ctxt& ctxt) const {
    if (prms.empty()) {
        std::cout << std::endl;
        return std::nullopt;
    }

    for (Expn_ptr expn : prms) {
        std::cout << to_string(expn->eval(defs,ctxt)) << std::endl;
    }
    return std::nullopt;
}

/*
Valu Defn::call(const Defs& defs, Ctxt& ctxt) const {

}*/


std::optional<Valu> SCal::exec(const Defs& defs, Ctxt& ctxt) const {

    for (int i = (int)defs.size()-1; i >= 0; --i) {
        Defn_ptr def = defs[i];
        if (def->name.compare(name) == 0) {
            if (def->args.size() != args.size()) {
                std::string msg = "Incorrect number of args found for function " 
                    + name + ": expected " + std::to_string(def->args.size()) + ", saw " 
                    + std::to_string(args.size()) + ".";
                throw DwislpyError { where(), msg };
            }

            Ctxt fctxt = { };
            for (int i = 0; i < (int)args.size(); ++i) fctxt[def->args[i]] = args[i]->eval(defs,ctxt);
            std::optional<Valu> rv = def->nest->exec(defs,fctxt);
            if (rv.has_value()) {
                return rv;
            } 
            return std::nullopt;
        }
    }


    std::string msg = "No function with name " + name + " found.";
    throw DwislpyError { where(), msg };
}

std::optional<Valu> Whle::exec(const Defs& defs, Ctxt& ctxt) const {
    while (predicate(expn->eval(defs,ctxt))) {
        std::optional<Valu> rv = nest->exec(defs,ctxt);
        if (rv.has_value()) {
            return rv;
        } 
    }
    return std::nullopt;
}

std::optional<Valu> Tern::exec(const Defs& defs, Ctxt& ctxt) const {
    if (predicate(expn->eval(defs,ctxt))) {
        std::optional<Valu> rv = nest_if->exec(defs,ctxt);
        if (rv.has_value()) {
            return rv;
        } 
    } else {
        std::optional<Valu> rv = nest_else->exec(defs,ctxt);
        if (rv.has_value()) {
            return rv;
        } 
    }
    return std::nullopt;
}

std::optional<Valu> Retn::exec(const Defs& defs, Ctxt& ctxt) const {
    return Valu { };
}

std::optional<Valu> RetE::exec(const Defs& defs, Ctxt& ctxt) const {
    Valu e = expn->eval(defs,ctxt);
    return e;
}

//
// Expn::eval
//
//  - evaluate DWISLPY expressions within a runtime context to determine their
//    (integer) value.
//

Valu ECal::eval(const Defs& defs, const Ctxt& ctxt) const {

    for (int i = (int)defs.size()-1; i >= 0; --i) {
        Defn_ptr def = defs[i];
        if (def->name.compare(name) == 0) {
            if (def->args.size() != args.size()) {
                std::string msg = "Incorrect number of args found for function " 
                    + name + ": expected " + std::to_string(def->args.size()) + ", saw " 
                    + std::to_string(args.size()) + ".";
                throw DwislpyError { where(), msg };
            }

            Ctxt fctxt = { };
            for (int i = 0; i < (int)args.size(); ++i) fctxt[def->args[i]] = args[i]->eval(defs,ctxt);
            std::optional<Valu> rv = def->nest->exec(defs,fctxt);
            if (rv.has_value()) {
                return rv.value();
            } 
            return Valu { };
        }
    }

    std::string msg = "No function with name " + name + " found.";
    throw DwislpyError { where(), msg };
}

Valu Plus::eval(const Defs& defs, const Ctxt& ctxt) const {
    Valu lv = left->eval(defs,ctxt);
    Valu rv = rght->eval(defs,ctxt);
    if (std::holds_alternative<int>(lv)
        && std::holds_alternative<int>(rv)) {
        int ln = std::get<int>(lv);
        int rn = std::get<int>(rv);
        return Valu {ln + rn};
    } else if (std::holds_alternative<std::string>(lv)
               && std::holds_alternative<std::string>(rv)) {
        std::string ls = std::get<std::string>(lv);
        std::string rs = std::get<std::string>(rv);
        return Valu {ls + rs};
    } else {
        std::string msg = "Run-time error: wrong operand type for plus.";
        throw DwislpyError { where(), msg };
    }        
}

Valu Conj::eval(const Defs& defs, const Ctxt& ctxt) const {
    Valu lv = left->eval(defs,ctxt);
    Valu rv = rght->eval(defs,ctxt);
    return predicate(lv) && predicate(rv);
}

Valu Disj::eval(const Defs& defs, const Ctxt& ctxt) const {
    Valu lv = left->eval(defs,ctxt);
    Valu rv = rght->eval(defs,ctxt);
    return predicate(lv) || predicate(rv);
}

Valu Less::eval(const Defs& defs, const Ctxt& ctxt) const {
    Valu lv = left->eval(defs,ctxt);
    Valu rv = rght->eval(defs,ctxt);
    if (std::holds_alternative<int>(lv)
        && std::holds_alternative<int>(rv)) {
        int ln = std::get<int>(lv);
        int rn = std::get<int>(rv);
        return Valu {ln < rn};
    } else {
        std::string msg = "Run-time error: wrong operand type for less than.";
        throw DwislpyError { where(), msg };
    }      
}

Valu LtEq::eval(const Defs& defs, const Ctxt& ctxt) const {
       Valu lv = left->eval(defs,ctxt);
    Valu rv = rght->eval(defs,ctxt);
    if (std::holds_alternative<int>(lv)
        && std::holds_alternative<int>(rv)) {
        int ln = std::get<int>(lv);
        int rn = std::get<int>(rv);
        return Valu {ln <= rn};
    } else {
        std::string msg = "Run-time error: wrong operand type for less than or equal to.";
        throw DwislpyError { where(), msg };
    }
}

Valu Eqal::eval(const Defs& defs, const Ctxt& ctxt) const {
    Valu lv = left->eval(defs,ctxt);
    Valu rv = rght->eval(defs,ctxt);
    if (std::holds_alternative<int>(lv)
        && std::holds_alternative<int>(rv)) {
        int ln = std::get<int>(lv);
        int rn = std::get<int>(rv);
        return Valu {ln == rn};
    } else if (std::holds_alternative<std::string>(lv)
               && std::holds_alternative<std::string>(rv)) {
        std::string ls = std::get<std::string>(lv);
        std::string rs = std::get<std::string>(rv);
        bool ret = (ls.compare(rs) == 0);
        return Valu {ret};
    } else {
        return Valu {false};
    }      
}

Valu Negt::eval(const Defs& defs, const Ctxt& ctxt) const {
    Valu ex = expn->eval(defs,ctxt);
    bool res = predicate(ex);
    return Valu {!res};  
}

Valu Mnus::eval(const Defs& defs, const Ctxt& ctxt) const {
    Valu lv = left->eval(defs,ctxt);
    Valu rv = rght->eval(defs,ctxt);
    if (std::holds_alternative<int>(lv)
        && std::holds_alternative<int>(rv)) {
        int ln = std::get<int>(lv);
        int rn = std::get<int>(rv);
        return Valu {ln - rn};
    } else {
        std::string msg = "Run-time error: wrong operand type for minus.";
        throw DwislpyError { where(), msg };
    }        
}

Valu Tmes::eval(const Defs& defs, const Ctxt& ctxt) const {
    Valu lv = left->eval(defs,ctxt);
    Valu rv = rght->eval(defs,ctxt);
    if (std::holds_alternative<int>(lv)
        && std::holds_alternative<int>(rv)) {
        int ln = std::get<int>(lv);
        int rn = std::get<int>(rv);
        return Valu {ln * rn};
    } else {
        // Exercise: make this work for (int,str) and (str,int).
        std::string msg = "Run-time error: wrong operand type for times.";
        throw DwislpyError { where(), msg };
    }        
}

Valu IDiv::eval(const Defs& defs, const Ctxt& ctxt) const {
    Valu lv = left->eval(defs,ctxt);
    Valu rv = rght->eval(defs,ctxt);
    if (std::holds_alternative<int>(lv)
        && std::holds_alternative<int>(rv)) {
        int ln = std::get<int>(lv);
        int rn = std::get<int>(rv);
        if (rn == 0) {
            throw DwislpyError { where(), "Run-time error: division by 0."};
        } else {
            return Valu {ln / rn};
        } 
    } else {
        std::string msg = "Run-time error: wrong operand type for quotient.";
        throw DwislpyError { where(), msg };
    }        
}

Valu IMod::eval(const Defs& defs, const Ctxt& ctxt) const {
    Valu lv = left->eval(defs,ctxt);
    Valu rv = rght->eval(defs,ctxt);
    if (std::holds_alternative<int>(lv)
        && std::holds_alternative<int>(rv)) {
        int ln = std::get<int>(lv);
        int rn = std::get<int>(rv);
        if (rn == 0) {
            throw DwislpyError { where(), "Run-time error: division by 0."};
        } else {
            return Valu {ln % rn};
        } 
    } else {
        std::string msg = "Run-time error: wrong operand type for remainder.";
        throw DwislpyError { where(), msg };
    }        
}
Valu Ltrl::eval([[maybe_unused]] const Defs& defs,
                [[maybe_unused]] const Ctxt& ctxt) const {
    return valu;
}

Valu Lkup::eval([[maybe_unused]] const Defs& defs, const Ctxt& ctxt) const {
    if (ctxt.count(name) > 0) {
        return ctxt.at(name);
    } else {
        std::string msg = "Run-time error: variable '" + name +"'";
        msg += "not defined.";
        throw DwislpyError { where(), msg };
    }
}

Valu Inpt::eval([[maybe_unused]] const Defs& defs, const Ctxt& ctxt) const {
    Valu v = expn->eval(defs,ctxt);
    if (std::holds_alternative<std::string>(v)) {
        //
        std::string prompt = std::get<std::string>(v);
        std::cout << prompt;
        //
        std::string vl;
        std::cin >> vl;
        //
        return Valu {vl};
    } else {
        std::string msg = "Run-time error: prompt is not a string.";
        throw DwislpyError { where(), msg };
    }
}

Valu IntC::eval([[maybe_unused]] const Defs& defs, const Ctxt& ctxt) const {
    //
    // The integer conversion operation does nothing in this
    // version of DWISLPY.
    //
    Valu v = expn->eval(defs,ctxt);
    if (std::holds_alternative<int>(v)) {
        return Valu {v};
    } else if (std::holds_alternative<std::string>(v)) {
        std::string s = std::get<std::string>(v);
        try {
            int i = std::stoi(s);
            return Valu {i};
        } catch (std::invalid_argument e) {
            std::string msg = "Run-time error: \""+s+"\"";
            msg += "cannot be converted to an int.";
            throw DwislpyError { where(), msg };
        }
    } else if (std::holds_alternative<bool>(v)) {
        bool b = std::get<bool>(v);
        return Valu {b ? 1 : 0};
    } else {
        std::string msg = "Run-time error: cannot convert to an int.";
        throw DwislpyError { where(), msg };
    }
}

Valu StrC::eval([[maybe_unused]] const Defs& defs, const Ctxt& ctxt) const {
    //
    // The integer conversion operation does nothing in this
    // version of DWISLPY.
    //
    Valu v = expn->eval(defs,ctxt);
    return Valu { to_string(v) };
}

//
// Expn::pred
//
/*
Bool Plus::pred(const Defs& defs, const Ctxt& ctxt) const {
    return left->pred(defs,ctxt) || rght->pred(defs,ctxt);
}*/

// * * * * *
//
// AST::output
//
// - Pretty printer for DWISLPY code represented in an AST.
//
// The code below is an implementation of a pretty printer. For each case
// of an AST node (each subclass) the `output` method provides the means for
// printing the code of the DWISLPY construct it represents.
//
//

void Prgm::output(std::ostream& os) const {
    for (Defn_ptr defn : defs) {
        defn->output(os);
    }
    main->output(os);
}

void Defn::output(std::ostream& os) const {
    os << "def " << name << "(";
    for (Name a : args) {
        os << a;
        if (a.compare(args.back()) != 0) {
            os << ", ";
        }
    }
    os << "):" << std::endl;
    nest->output(os);
}

void Blck::output(std::ostream& os, std::string indent) const {
    for (Stmt_ptr s : stmts) {
        s->output(os,indent);
    }
}

void Blck::output(std::ostream& os) const {
    for (Stmt_ptr s : stmts) {
        s->output(os);
    }
}

void Nest::output(std::ostream& os, std::string indent) const {
    blck->output(os,indent + "    ");
}

void Nest::output(std::ostream& os) const {
    blck->output(os,"    ");
}


void Stmt::output(std::ostream& os) const {
    output(os,"");
}

void Asgn::output(std::ostream& os, std::string indent) const {
    os << indent;
    os << name << " = ";
    expn->output(os);
    os << std::endl;
}

void Pass::output(std::ostream& os, std::string indent) const {
    os << indent << "pass" << std::endl;
}

void Prnt::output(std::ostream& os, std::string indent) const {
    os << indent;
    os << "print";
    os << "(";
    for (Expn_ptr a : prms) {
        a->output(os);
        if (a != prms.back()) {
            os << ", ";
        }
    }
    os << ")";
    os << std::endl;
}

void PlEq::output(std::ostream& os, std::string indent) const {
    os << indent;
    os << name;
    os << " += ";
    expn->output(os);
    os << std::endl;
}

void MiEq::output(std::ostream& os, std::string indent) const {
    os << indent;
    os << name;
    os << " -= ";
    expn->output(os);
    os << std::endl;
}

void Whle::output(std::ostream& os, std::string indent) const {
    os << indent;
    os << "while";
    os << "(";
    expn->output(os);
    os << "):";
    os << std::endl;
    nest->output(os, indent);
}

void ECal::output(std::ostream& os) const {
    os << name;
    os << "(";
    for (Expn_ptr a : args) {
        a->output(os);
        if (a != args.back()) {
            os << ", ";
        }
    }
    os << ")";
}

void SCal::output(std::ostream& os, std::string indent) const {
    os << indent;
    os << name;
    os << "(";
    for (Expn_ptr a : args) {
        a->output(os);
        if (a != args.back()) {
            os << ", ";
        }
    }
    os << ")";
    os << std::endl;
}

void Tern::output(std::ostream& os, std::string indent) const {
    os << indent;
    os << "if ";
    expn->output(os);
    os << ":" << std::endl;
    nest_if->output(os, indent);
    os << "else:" << std::endl;
    nest_else->output(os, indent);
    os << std::endl;
}

void Retn::output(std::ostream& os, std::string indent) const {
    os << indent;
    os << "return" << std::endl;
}

void RetE::output(std::ostream& os, std::string indent) const {
    os << indent;
    os << "return ";
    expn->output(os);
    os << std::endl;
}

void Plus::output(std::ostream& os) const {
    os << "(";
    left->output(os);
    os << " + ";
    rght->output(os);
    os << ")";
}

void Mnus::output(std::ostream& os) const {
    os << "(";
    left->output(os);
    os << " - ";
    rght->output(os);
    os << ")";
}

void Tmes::output(std::ostream& os) const {
    os << "(";
    left->output(os);
    os << " * ";
    rght->output(os);
    os << ")";
}

void IDiv::output(std::ostream& os) const {
    os << "(";
    left->output(os);
    os << " // ";
    rght->output(os);
    os << ")";
}

void IMod::output(std::ostream& os) const {
    os << "(";
    left->output(os);
    os << " % ";
    rght->output(os);
    os << ")";
}

void Conj::output(std::ostream& os) const {
    os << "(";
    left->output(os);
    os << " and ";
    rght->output(os);
    os << ")";
}

void Disj::output(std::ostream& os) const {
    os << "(";
    left->output(os);
    os << " or ";
    rght->output(os);
    os << ")";
}

void Less::output(std::ostream& os) const {
    os << "(";
    left->output(os);
    os << " < ";
    rght->output(os);
    os << ")";
}

void LtEq::output(std::ostream& os) const {
    os << "(";
    left->output(os);
    os << " <= ";
    rght->output(os);
    os << ")";
}

void Eqal::output(std::ostream& os) const {
    os << "(";
    left->output(os);
    os << " == ";
    rght->output(os);
    os << ")";
}

void Negt::output(std::ostream& os) const {
    os << "not(";
    expn->output(os);
    os << ")";
}


void Ltrl::output(std::ostream& os) const {
    os << to_repr(valu);
}

void Lkup::output(std::ostream& os) const {
    os << name;
}

void Inpt::output(std::ostream& os) const {
    os << "input(";
    expn->output(os);
    os << ")";
}

void IntC::output(std::ostream& os) const {
    os << "int(";
    expn->output(os);
    os << ")";
}

void StrC::output(std::ostream& os) const {
    os << "str(";
    expn->output(os);
    os << ")";
}


// * * * * *
//
// AST::dump
//
// - Dumps the AST of DWISLPY code.
//
// The code below dumps the contents of an AST as a series of lines
// indented and headed by the AST node type and its subcontents.
//

void dump_indent(int level) {
    //std::string indent { level, std::string {"    "} };
    for (int i=0; i<level; i++) {
        std::cout << "    ";
    }
}

void Prgm::dump(int level) const {
    dump_indent(level);
    std::cout << "PRGM" << std::endl;
    for (Defn_ptr defn : defs) {
        defn->dump(level+1);
    }
    main->dump(level+1);
}

void Defn::dump(int level) const {
    dump_indent(level);
    std::cout << "DEFN" << std::endl;
    for (Name n : args) {
        dump_indent(level+1);
        std::cout << n << std::endl;
    }
    nest->dump(level+1);
}

void Blck::dump(int level) const {
    dump_indent(level);
    std::cout << "BLCK" << std::endl;
    for (Stmt_ptr stmt : stmts) {
        stmt->dump(level+1);
    }
}

void Nest::dump(int level) const {
    dump_indent(level);
    std::cout << "NEST" << std::endl;
    blck->dump(level+1);
}

void Asgn::dump(int level) const {
    dump_indent(level);
    std::cout << "ASGN" << std::endl;
    dump_indent(level+1);
    std::cout << name << std::endl;
    expn->dump(level+1);
}

void Prnt::dump(int level) const {
    dump_indent(level);
    std::cout << "PRNT" << std::endl;
    for (Expn_ptr expn : prms) {
        expn->dump(level+1);
    }
}

void PlEq::dump(int level) const {
    dump_indent(level);
    std::cout << "PLEQ" << std::endl;
    dump_indent(level+1);
    std::cout << name << std::endl;
    expn->dump(level+1);
}

void MiEq::dump(int level) const {
    dump_indent(level);
    std::cout << "MIEQ" << std::endl;
    dump_indent(level+1);
    std::cout << name << std::endl;
    expn->dump(level+1);
}

void Whle::dump(int level) const {
    dump_indent(level);
    std::cout << "WHLE" << std::endl;
    expn->dump(level+1);
    nest->dump(level+1);
}

void Tern::dump(int level) const {
    dump_indent(level);
    std::cout << "TERN" << std::endl;
    expn->dump(level+1);
    nest_if->dump(level+1);
    nest_else->dump(level+1);
}

void ECal::dump(int level) const {
    dump_indent(level);
    std::cout << "CALL" << std::endl;
    dump_indent(level+1);
    std::cout << name << std::endl;
    for (Expn_ptr expn : args) {
        expn->dump(level+1);
    }
}

void SCal::dump(int level) const {
    dump_indent(level);
    std::cout << "CALL" << std::endl;
    dump_indent(level+1);
    std::cout << name << std::endl;
    for (Expn_ptr expn : args) {
        expn->dump(level+1);
    }
}

void Retn::dump(int level) const {
    dump_indent(level);
    std::cout << "RETURN" << std::endl;
}

void RetE::dump(int level) const {
    dump_indent(level);
    std::cout << "RETURN" << std::endl;
    expn->dump(level+1);
}

void Pass::dump(int level) const {
    dump_indent(level);
    std::cout << "PASS" << std::endl;
}

void Plus::dump(int level) const {
    dump_indent(level);
    std::cout << "PLUS" << std::endl;
    left->dump(level+1);
    rght->dump(level+1);
}

void Mnus::dump(int level) const {
    dump_indent(level);
    std::cout << "MNUS" << std::endl;
    left->dump(level+1);
    rght->dump(level+1);
}

void Tmes::dump(int level) const {
    dump_indent(level);
    std::cout << "TMES" << std::endl;
    left->dump(level+1);
    rght->dump(level+1);
}

void IDiv::dump(int level) const {
    dump_indent(level);
    std::cout << "IDIV" << std::endl;
    left->dump(level+1);
    rght->dump(level+1);
}

void IMod::dump(int level) const {
    dump_indent(level);
    std::cout << "IDIV" << std::endl;
    left->dump(level+1);
    rght->dump(level+1);
}

void Conj::dump(int level) const {
    dump_indent(level);
    std::cout << "CONJ" << std::endl;
    left->dump(level+1);
    rght->dump(level+1);
}

void Disj::dump(int level) const {
    dump_indent(level);
    std::cout << "PLUS" << std::endl;
    left->dump(level+1);
    rght->dump(level+1);
}

void Less::dump(int level) const {
    dump_indent(level);
    std::cout << "LESS" << std::endl;
    left->dump(level+1);
    rght->dump(level+1);
}

void LtEq::dump(int level) const {
    dump_indent(level);
    std::cout << "LTEQ" << std::endl;
    left->dump(level+1);
    rght->dump(level+1);
}

void Eqal::dump(int level) const {
    dump_indent(level);
    std::cout << "EQAL" << std::endl;
    left->dump(level+1);
    rght->dump(level+1);
}

void Negt::dump(int level) const {
    dump_indent(level);
    std::cout << "Negt" << std::endl;
    expn->dump(level+1);
}

void Ltrl::dump(int level) const {
    dump_indent(level);
    std::cout << "LTRL" << std::endl;
    dump_indent(level+1);
    std::cout << to_repr(valu) << std::endl;
}

void Lkup::dump(int level) const {
    dump_indent(level);
    std::cout << "LKUP" << std::endl;
    dump_indent(level+1);
    std::cout << name << std::endl;
}

void Inpt::dump(int level) const {
    dump_indent(level);
    std::cout << "INPT" << std::endl;
    expn->dump(level+1);
}

void IntC::dump(int level) const {
    dump_indent(level);
    std::cout << "INTC" << std::endl;
    expn->dump(level+1);
}

void StrC::dump(int level) const {
    dump_indent(level);
    std::cout << "STRC" << std::endl;
    expn->dump(level+1);
}
