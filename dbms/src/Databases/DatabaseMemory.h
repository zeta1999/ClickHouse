#pragma once

#include <Databases/DatabasesCommon.h>
#include <Common/escapeForFileName.h>
#include <Parsers/ASTCreateQuery.h>


namespace Poco { class Logger; }


namespace DB
{

/** A non-persistent database to store temporary data.
  * It doesn't make any manipulations with filesystem.
  * All tables are created by calling code.
  * TODO: Maybe DatabaseRuntime is more suitable class name.
  */
class DatabaseMemory : public DatabaseWithOwnTablesBase
{
public:
    DatabaseMemory(const String & name_);

    String getEngineName() const override { return "Memory"; }

    void createTable(
        const Context & context,
        const String & table_name,
        const StoragePtr & table,
        const ASTPtr & query) override;

    void removeTable(
        const Context & context,
        const String & table_name) override;

    ASTPtr getCreateTableQueryImpl(const Context & /*context*/, const String & name, bool throw_on_error) const override;
    ASTPtr getCreateDatabaseQuery(const Context & /*context*/) const override;

    /// DatabaseMemory allows to create tables, which store data on disk.
    /// It's needed to create such tables in default database of clickhouse-local.
    /// TODO May be it's better to use DiskMemory for such tables.
    ///      To save data on disk it's possible to explicitly CREATE DATABASE db ENGINE=Ordinary in clickhouse-local.
    String getTableDataPath(const String & table_name) const override { return data_path + escapeForFileName(table_name) + "/"; }
    String getTableDataPath(const ASTCreateQuery & query) const override { return getTableDataPath(query.table); }

private:
    String data_path;
    using NameToASTCreate = std::unordered_map<String, ASTPtr>;
    NameToASTCreate create_queries;
};

}
