# Legal notice and project scope

This file explains what this repository is, what it does not contain, and the
boundaries contributions must respect. It is a description of the project's
intent and policies. It is not legal advice, and nothing here is a statement
that any particular use of this repository is lawful in your jurisdiction.

## What the project is

Halo 2 Decompilation is an independent, non-commercial preservation and
research project. Contributors study the retail Halo 2 executable for the
original Xbox through their own reverse-engineering and analysis, and write
new C and C++ source code that recreates its program logic. The project's
tools compare the output of that source, built with the period-correct
compiler, against the retail executable, so that the recreated logic can be
checked function by function.

The project is intended for preservation, research, education and
interoperability.

## Affiliation and trademarks

This project is not affiliated with, endorsed by, sponsored by or approved by
Microsoft, Bungie, 343 Industries, Activision or any other rights holder.

Halo, Halo 2, Xbox, Bungie, Microsoft, 343 Industries and related names and
marks are trademarks or registered trademarks of their respective owners.
They are used here only to identify the software being studied.

## What the repository does not contain

The repository does not contain, and does not distribute:

- the Halo 2 retail game, its executable (`default.xbe`), or any disc image;
- original game assets: maps, textures, models, audio, cinematics or other
  game data;
- the Microsoft Xbox SDK (XDK), its compiler, libraries or headers, or any
  other proprietary Microsoft development tool;
- original or leaked Halo 2 source code;
- leaked symbols, program databases (PDBs), linker maps, or confidential or
  internal Microsoft, Bungie or 343 Industries documents or builds.

The `.gitignore` excludes executables, disc images, archives, symbol files and
the folders where contributors keep their own local copies of these files, so
that they are not committed by accident.

## What users and contributors must supply

Anyone who uses the project's tools must supply any required original game
material themselves, from a copy they are legally entitled to use. The
project does not provide game files and cannot help anyone obtain them.

The project does not distribute the Microsoft Xbox SDK/XDK or any proprietary
Microsoft development tools. Contributors are responsible for ensuring that
any development tools they use are obtained and used lawfully. The
maintainers cannot provide, link to, or assist with obtaining proprietary
SDK/XDK materials.

Contributors are responsible for complying with the laws that apply to them.

## Source of the code in this repository

The code in this repository is intended to be independently recreated source:
written by contributors from their own analysis of the retail executable and
the program's observable behaviour. It is not, and is not intended to
contain, Bungie's original source code or code copied from leaked material.

Contributions derived from leaked, confidential, stolen or NDA-restricted
material, or from any other unauthorised source, are not accepted. The
contribution policy is in [CONTRIBUTING.md](CONTRIBUTING.md), and the
project's account of where its information comes from, including known
issues that are being remediated, is in [PROVENANCE.md](PROVENANCE.md).

## Licence scope

The repository's own contributions are licensed under the GNU General Public
License, version 3 or (at your option) any later version (GPL-3.0-or-later;
see [LICENSE](LICENSE)), except where a file or the README says otherwise.
Versions published before the change on 2026-10-27 were released under
CC0 1.0, and copies of those versions stay under CC0.

- The GPL applies only to material that the contributors have the right to
  license. It covers the contributors' own work in this repository and
  nothing else.
- It grants no rights in any part of Halo 2, or in any intellectual property
  of Microsoft, Bungie, 343 Industries, Activision or any other third party.
  The earlier CC0 release did not place any of that into the public domain,
  and could not waive anyone else's rights.
- Nothing in this repository grants any licence to Halo 2's game content,
  code, trademarks or other proprietary material. Those remain the property
  of their respective owners.
- The licence includes one additional permission under section 7 of the GPL,
  for linking or combining with the libraries of the Microsoft Xbox
  Development Kit. Its text is in the README's
  [Additional permission for the Xbox Development Kit](README.md#additional-permission-for-the-xbox-development-kit).
- Some data in the repository comes from third-party projects under their own
  licences; the README's Licence section lists them.

## Contact

Rights holders, or anyone else with a concern about specific material in this
repository, can contact the maintainers by opening an issue at
https://github.com/kirklandsig/halo2-decompiled/issues, or privately through
the repository owner's GitHub profile. Please identify the specific files or
commits concerned. The maintainers will look into any such report promptly,
and will remove material where that is appropriate.
