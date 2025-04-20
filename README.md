# pg_migration_graph
PostgreSQL migration visualizer - Transform SQL schema changes into weighted directed graphs using AST parsing and graph theory.

## Features

- [See the project board](https://github.com/users/danmxli/projects/11)
- Visualize schema evolution over time
- Build dependency graphs from migration scripts

## Development

### Project Structure

- `src/` - Source code
- `tests/` - Testing methodology

### Testing

Run the tests with:

```bash
make test
```

## Acknowledgments

- [libpg_query](https://github.com/pganalyze/libpg_query) - PostgreSQL parser used for SQL analysis
- [jansson](https://github.com/akheron/jansson) - C library for encoding, decoding and manipulating JSON data
