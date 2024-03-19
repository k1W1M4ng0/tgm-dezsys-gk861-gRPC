from google.protobuf import timestamp_pb2 as _timestamp_pb2
from google.protobuf.internal import containers as _containers
from google.protobuf import descriptor as _descriptor
from google.protobuf import message as _message
from typing import ClassVar as _ClassVar, Iterable as _Iterable, Mapping as _Mapping, Optional as _Optional, Union as _Union

DESCRIPTOR: _descriptor.FileDescriptor

class getDataRequest(_message.Message):
    __slots__ = ("id",)
    ID_FIELD_NUMBER: _ClassVar[int]
    id: int
    def __init__(self, id: _Optional[int] = ...) -> None: ...

class WarehouseData(_message.Message):
    __slots__ = ("warehouseApplicationID", "warehouseID", "warehouseName", "warehouseAddress", "warehousePostalCode", "warehouseCity", "warehouseCountry", "timestamp", "productData")
    WAREHOUSEAPPLICATIONID_FIELD_NUMBER: _ClassVar[int]
    WAREHOUSEID_FIELD_NUMBER: _ClassVar[int]
    WAREHOUSENAME_FIELD_NUMBER: _ClassVar[int]
    WAREHOUSEADDRESS_FIELD_NUMBER: _ClassVar[int]
    WAREHOUSEPOSTALCODE_FIELD_NUMBER: _ClassVar[int]
    WAREHOUSECITY_FIELD_NUMBER: _ClassVar[int]
    WAREHOUSECOUNTRY_FIELD_NUMBER: _ClassVar[int]
    TIMESTAMP_FIELD_NUMBER: _ClassVar[int]
    PRODUCTDATA_FIELD_NUMBER: _ClassVar[int]
    warehouseApplicationID: str
    warehouseID: str
    warehouseName: str
    warehouseAddress: str
    warehousePostalCode: str
    warehouseCity: str
    warehouseCountry: str
    timestamp: str
    productData: _containers.RepeatedCompositeFieldContainer[Product]
    def __init__(self, warehouseApplicationID: _Optional[str] = ..., warehouseID: _Optional[str] = ..., warehouseName: _Optional[str] = ..., warehouseAddress: _Optional[str] = ..., warehousePostalCode: _Optional[str] = ..., warehouseCity: _Optional[str] = ..., warehouseCountry: _Optional[str] = ..., timestamp: _Optional[str] = ..., productData: _Optional[_Iterable[_Union[Product, _Mapping]]] = ...) -> None: ...

class Product(_message.Message):
    __slots__ = ("productID", "productName", "productCategory", "productQuantity", "productUnit")
    PRODUCTID_FIELD_NUMBER: _ClassVar[int]
    PRODUCTNAME_FIELD_NUMBER: _ClassVar[int]
    PRODUCTCATEGORY_FIELD_NUMBER: _ClassVar[int]
    PRODUCTQUANTITY_FIELD_NUMBER: _ClassVar[int]
    PRODUCTUNIT_FIELD_NUMBER: _ClassVar[int]
    productID: str
    productName: str
    productCategory: str
    productQuantity: int
    productUnit: str
    def __init__(self, productID: _Optional[str] = ..., productName: _Optional[str] = ..., productCategory: _Optional[str] = ..., productQuantity: _Optional[int] = ..., productUnit: _Optional[str] = ...) -> None: ...
