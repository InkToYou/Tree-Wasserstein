.PHONY: install
install:
	pipenv install Pipfile && pipenv run pip install ../Tree-Wasserstein

.PHONY: test
test:
	pipenv run pytest .
