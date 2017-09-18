//
// Created by l.czerwinski on 9/21/17.
//

#include "Dao.h"

using namespace dao;

dao_error dao_exception::code()
{
    return err;
}

dao_exception::dao_exception(dao_error error, const char *msg)
{
    err = error;
    message = std::string("internal database problem: ");
    message += std::string(msg);
}

dao_exception::dao_exception(dao_error error) :
    dao_exception(error, "")
{
}

dao_exception::dao_exception(const char *msg) :
    dao_exception(dao_error::internal_error, "MSG")
{

}