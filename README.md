# Java ClassFile Parser (JCFP)

A library to parse and manage Java ClassFiles

## License

This project is licensed under the `GNU AGPL-3.0`

Read the file `LICENSE` for more information

## Considerations

This library is inspired by [libjnif](https://github.com/acuarica/jnif/tree/main/src-libjnif)

After making my first ClassFile parser, it felt like it could be designed much better

The `jnif` library seemed to be better, both in design in usability, but it felt overengineered
due to using its own custom allocators, iterators, etc

This library is meant to be a better and more ergonomic version of my initial parser, but also
not overengineered
