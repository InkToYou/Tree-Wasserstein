.PHONY: test

install:
	pipenv run pip install ../Tree-Wasserstein

test:
	pipenv run pytest .
