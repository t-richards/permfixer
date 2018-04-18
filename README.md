# permfixer

[![CircleCI](https://circleci.com/gh/t-richards/permfixer.svg?style=svg)](https://circleci.com/gh/t-richards/permfixer)

Permissions fixer utility for Unix and GNU/Linux.

## Dependencies

 - Make
 - C compiler
 - ACL development headers (Debian: `libacl1-dev`, Fedora: `libacl-devel`)

## Building from source

```
$ make
```

## Usage

```
$ permfixer [opts] <directory path>
options:
  -h, --help           Show this help message
  -d, --dperm <perm>   Specify directory permissions
                       Default: 0755
  -f, --fperm <perm>   Specify file permissions
                       Default: 0644
  -g, --group <group>  Specify an alternate group
                       Default: 100
  -o, --owner <owner>  Specify an alternate owner
                       Default: 1000

```

## Examples

1. Fix permissions on a janky web root

```
# permfixer --dperm 2775 --fperm 0664 --owner www-data --group www-data /var/www/site
```
