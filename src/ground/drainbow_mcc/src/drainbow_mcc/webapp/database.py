from flask_sqlalchemy import SQLAlchemy

from ..common.models import ModelBase

db = SQLAlchemy(metadata=ModelBase.metadata)
