# permfixer

[![CircleCI](https://circleci.com/gh/t-richards/permfixer.svg?style=svg)](https://circleci.com/gh/t-richards/permfixer)

Permissions fixer utility for Unix and GNU/Linux. Basically a special-purpose chown+chmod+setfacl.

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
                       Current value: 0755
  -f, --fperm <perm>   Specify file permissions
                       Current value: 0644
  -g, --group <group>  Specify an alternate group owner
                       Current value: 100
  -u, --user <user>    Specify an alternate user owner
                       Current value: 1000

```

## Examples

1. Fix permissions on a janky web root

```
# permfixer --dperm 2775 --fperm 0664 --owner www-data --group www-data /var/www/site
```
