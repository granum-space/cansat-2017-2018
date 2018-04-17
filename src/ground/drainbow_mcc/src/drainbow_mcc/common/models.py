from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy import Column, BigInteger, String

ModelBase = declarative_base()


class Test(ModelBase):
    __tablename__ = "test"

    id = Column(BigInteger, primary_key=True)
    value = Column(String, nullable=True)


