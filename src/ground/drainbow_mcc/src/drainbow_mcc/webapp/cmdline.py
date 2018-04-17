import click
from .app_singleton import app
from .database import db
from sqlalchemy import create_engine


def _buld_engine():
    return create_engine(app.config["SQLALCHEMY_DATABASE_URI"])



@app.cli.command()
def create_database():
    engine = _buld_engine()
    db.metadata.drop_all(engine)
    db.metadata.create_all(engine)

