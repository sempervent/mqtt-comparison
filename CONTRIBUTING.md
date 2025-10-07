# Contributing to MQTT Comparison

Thank you for your interest in contributing! This document provides guidelines for contributing to the project.

## Getting Started

1. Fork the repository
2. Clone your fork
3. Create a feature branch
4. Make your changes
5. Test your changes
6. Submit a pull request

## Development Setup

### Prerequisites

- Docker and Docker Compose
- Python 3.11+
- Git

### Initial Setup

```bash
git clone https://github.com/YOUR_USERNAME/mqtt-comparison.git
cd mqtt-comparison
./smoke.sh
docker-compose up -d
```

## Contribution Areas

### Adding Language Implementations

Follow the phase order:
1. Python (Phase 2) - Complete
2. Rust (Phase 3)
3. C/C++ (Phase 4)
4. Julia/R/C# (Phase 5)

Each implementation should include:
- Publisher with CLI
- Subscriber with CLI
- JSON encoding support
- MessagePack encoding support
- README with usage examples
- Integration with benchmark harness

### Adding Encoding Formats

Phase 6 focuses on:
- CBOR support
- Protocol Buffers support

### Improving Benchmarks

Phase 7 covers:
- Enhanced metrics
- Visualization
- Analysis tools
- Documentation

## Code Guidelines

### General Principles

- Keep changes focused and minimal
- Write clear commit messages
- Add tests where appropriate
- Update documentation
- Follow existing code style

### Python

- Follow PEP 8
- Use type hints
- Add docstrings
- Keep functions small and focused

### Rust

- Follow Rust conventions
- Use `rustfmt`
- Run `clippy`
- Add inline documentation

### C/C++

- Follow project style
- Document functions
- Handle errors gracefully
- Avoid memory leaks

## Testing

### Before Submitting

1. Run smoke test:
```bash
./smoke.sh
```

2. Test MQTT functionality:
```bash
docker-compose up -d
# Test your implementation
docker-compose down
```

3. Run benchmarks if applicable:
```bash
cd benchmarks
python3 benchmark.py --languages <your_language>
```

### CI Checks

All pull requests must pass:
- Smoke test
- Docker Compose test
- Python syntax checks
- Structure verification

## Pull Request Process

1. Update documentation
2. Add phase summary if completing a phase
3. Ensure CI passes
4. Request review
5. Address feedback
6. Merge when approved

### PR Title Format

- `[Phase X] Add <language> implementation`
- `[Docs] Update <documentation>`
- `[Fix] Resolve <issue>`

### PR Description

Include:
- What was changed
- Why it was changed
- How to test
- Related issues

## Commit Messages

Use clear, descriptive commit messages:

```
[Phase X] Add Rust pub/sub with JSON encoding

- Implement publisher with JSON support
- Implement subscriber with JSON support
- Add Cargo.toml with dependencies
- Update benchmark harness
```

## Documentation

### Update When

- Adding new language
- Changing architecture
- Adding features
- Fixing bugs

### Documentation Files

- `README.md` - Project overview
- `docs/ARCHITECTURE.md` - System design
- `docs/DEVELOPMENT.md` - Development guide
- `docs/_summaries/` - Phase completion
- Language-specific READMEs

## Phase Completion

When completing a phase:

1. Ensure all functionality works
2. Update all relevant documentation
3. Create phase summary:
```bash
./summarize_phase.py <phase> "Description of work"
```
4. Submit PR with `[Phase X Complete]` prefix

## Getting Help

- Open an issue for bugs
- Start a discussion for questions
- Check existing documentation
- Review similar implementations

## Code of Conduct

- Be respectful and inclusive
- Focus on constructive feedback
- Help others learn and grow
- Celebrate contributions

## License

By contributing, you agree that your contributions will be licensed under the MIT License.
