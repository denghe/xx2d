//#include "All.h"
//
//namespace xxx
//{
//    CdGrid::CdGrid()
//    {
//        cellRadius = { 0, 0 };
//        cellDiameter = { 0, 0 };
//        gridDiameter = { 0, 0 };
//        rowCount = 0;
//        columnCount = 0;
//        autoFlag = 0;
//    }
//
//    CdGrid::~CdGrid()
//    {
//        for( int i = 0; i < items.Size(); ++i )
//        {
//            delete items[ i ];
//        }
//        for( int i = 0; i < freeItems.Size(); ++i )
//        {
//            delete freeItems[ i ];
//        }
//    }
//
//    void CdGrid::Reserve( int _capacity )
//    {
//        for( int i = 0; i < _capacity; ++i )
//        {
//            freeItems.Push( new CdItem() );
//        }
//    }
//
//    void CdGrid::Init( CdSize _gridDiameter, int _rowCount, int _columnCount )
//    {
//        Clear();
//        gridDiameter = _gridDiameter;
//        rowCount = _rowCount;
//        columnCount = _columnCount;
//        cellDiameter = { _gridDiameter.w / _columnCount, _gridDiameter.h / _rowCount };
//        cellRadius = { cellDiameter.w / 2, cellDiameter.h / 2 };
//        cells.Resize( _rowCount * _columnCount );
//        for( int ri = 0; ri < _rowCount; ++ri )
//        {
//            for( int ci = 0; ci < _columnCount; ++ci )
//            {
//                auto &c = cells[ ri*_columnCount + ci ];
//                c.ci = ci;
//                c.ri = ri;
//            }
//        }
//    }
//
//    CdItem* CdGrid::CreateItem()
//    {
//        CdItem* rtv;
//        if( freeItems.Size() )
//        {
//            rtv = freeItems.TopPop();
//            assert( rtv->cells.Size() == 0 );
//        }
//        else
//        {
//            rtv = new CdItem();
//        }
//        rtv->parent = this;
//        rtv->idx = items.Size();
//        items.Push( rtv );
//        return rtv;
//    }
//
//    void CdGrid::Clear()
//    {
//        for( int i = 0; i < items.Size(); ++i )
//        {
//            auto& p = items[ i ];
//            p->cells.Clear();
//            p->parent = nullptr;
//            freeItems.Push( p );
//        }
//        items.Clear();
//        for( int i = 0; i < cells.Size(); ++i )
//        {
//            cells[ i ].items.Clear();
//        }
//    }
//
//
//    void CdGrid::IncreaseFlag()
//    {
//        ++autoFlag;
//        if( autoFlag ) return;
//        autoFlag = 1;
//        for( int i = 0; i < items.Size(); ++i )
//        {
//            items[ i ]->flag = 0;
//        }
//        for( int i = 0; i < freeItems.Size(); ++i )
//        {
//            freeItems[ i ]->flag = 0;
//        }
//    }
//
//
//
//
//    bool CdGrid::CheckCollision( CdItem* _a, CdItem* _b )
//    {
//        uint r;
//        r = _a->radius.w + _b->radius.w;
//        if( (uint)( _a->pos.x - _b->pos.x + r ) > r + r ) return false;
//        r = _a->radius.h + _b->radius.h;
//        if( (uint)( _a->pos.y - _b->pos.y + r ) > r + r ) return false;
//        return true;
//    }
//
//    bool CdGrid::CheckCollision( CdItem* _a, CdPoint _pos )
//    {
//        return _a->pos.x - _a->radius.w <= _pos.x
//            && _a->pos.x + _a->radius.w >= _pos.x
//            && _a->pos.y - _a->radius.h <= _pos.y
//            && _a->pos.y + _a->radius.h >= _pos.y;
//    }
//
//    int CdGrid::GetItems( List<CdItem*>& _container, CdPoint const& _pos )
//    {
//        _container.Clear();
//        int ci = _pos.x / cellDiameter.w;
//        int ri = _pos.y / cellDiameter.h;
//        auto c = &cells[ ri * columnCount + ci ];
//        for( int i = 0; i < c->items.Size(); ++i )
//        {
//            auto& o = c->items[ i ]->value;
//            if( CheckCollision( o, _pos ) )
//            {
//                _container.Push( o );
//            }
//        }
//        return _container.Size();
//    }
//
//    CdItem* CdGrid::GetItem( CdPoint const& _pos )
//    {
//        int ci = _pos.x / cellDiameter.w;
//        int ri = _pos.y / cellDiameter.h;
//        auto c = &cells[ ri * columnCount + ci ];
//        for( int i = 0; i < c->items.Size(); ++i )
//        {
//            auto& o = c->items[ i ]->value;
//            if( CheckCollision( o, _pos ) )
//            {
//                return o;
//            }
//        }
//        return nullptr;
//    }
//
//
//
//
//
//
//    CdItem::CdItem()
//    {
//        parent = nullptr;
//        flag = 0;
//    }
//
//    CdItem::~CdItem()
//    {
//    }
//
//    void CdItem::Init( CdSize const& _radius, CdPoint const& _pos, int _groupId /*= 0*/, uint _layerMask /*= 0xFFFFFFFFu*/, void* _userData /*= nullptr */ )
//    {
//        assert( parent );
//        radius = _radius;
//        pos = _pos;
//        groupId = _groupId;
//        layerMask = _layerMask;
//        userData = _userData;
//        ci1 = ci2 = ri1 = ri2 = -1;
//    }
//
//    void CdItem::Destroy()
//    {
//        assert( parent );
//        assert( parent->items.Find( this ) >= 0 );
//        for( int i = 0; i < cells.Size(); ++i )
//        {
//            auto& pair = cells[ i ];
//            pair.first->items.Erase( pair.second );
//        }
//        cells.Clear();
//        parent->items.Top()->idx = idx;
//        parent->items.EraseFast( idx );
//        parent->freeItems.Push( this );
//        parent = nullptr;
//    }
//
//    void CdItem::Index()
//    {
//        assert( parent && ci1 == -1 );  // ensure Inited
//
//        for( int i = 0; i < cells.Size(); ++i )
//        {
//            auto& pair = cells[ i ];
//            pair.first->items.Erase( pair.second );
//        }
//        cells.Clear();
//
//        CdPoint _p = { pos.x - radius.w, pos.y - radius.h };
//        int _ci1 = _p.x / parent->cellDiameter.w;
//        int _ri1 = _p.y / parent->cellDiameter.h;
//        assert( _ci1 >= 0 && _ci1 < parent->columnCount && _ri1 >= 0 && _ri1 < parent->rowCount );
//
//        _p = { pos.x + radius.w, pos.y + radius.h };
//        int _ci2 = _p.x / parent->cellDiameter.w;
//        int _ri2 = _p.y / parent->cellDiameter.h;
//        assert( _ci2 >= 0 && _ci2 < parent->columnCount && _ri2 >= 0 && _ri2 < parent->rowCount );
//
//        for( int ri = _ri1; ri <= _ri2; ++ri )
//        {
//            for( int ci = _ci1; ci <= _ci2; ++ci )
//            {
//                auto c = &parent->cells[ ri * parent->columnCount + ci ];
//                cells.Push( std::make_pair( c, c->items.Insert( this ) ) );
//            }
//        }
//
//        ci1 = _ci1;
//        ci2 = _ci2;
//        ri1 = _ri1;
//        ri2 = _ri2;
//    }
//
//    void CdItem::Update( CdPoint const& _pos )
//    {
//        pos = _pos;
//
//        CdPoint p = { _pos.x - radius.w, _pos.y - radius.h };
//        int _ci1 = p.x / parent->cellDiameter.w;
//        int _ri1 = p.y / parent->cellDiameter.h;
//        assert( _ci1 >= 0 && _ci1 < parent->columnCount && _ri1 >= 0 && _ri1 < parent->rowCount );
//
//        p = { _pos.x + radius.w, _pos.y + radius.h };
//        int _ci2 = p.x / parent->cellDiameter.w;
//        int _ri2 = p.y / parent->cellDiameter.h;
//        assert( _ci2 >= 0 && _ci2 < parent->columnCount && _ri2 >= 0 && _ri2 < parent->rowCount );
//
//        if( ci1 == _ci1
//            && ci2 == _ci2
//            && ri1 == _ri1
//            && ri2 == _ri2 ) return;
//
//        ci1 = _ci1;
//        ci2 = _ci2;
//        ri1 = _ri1;
//        ri2 = _ri2;
//
//        for( int i = cells.Size() - 1; i >= 0; --i )
//        {
//            auto& c = *cells[ i ].first;
//            auto& node = cells[ i ].second;
//            if( c.ri < _ri1 || c.ri > _ri2 || c.ci < _ci1 || c.ci > _ci2 )
//            {
//                c.items.Erase( node );
//                cells.Erase( i );
//            }
//        }
//
//        for( int ri = _ri1; ri <= _ri2; ++ri )
//        {
//            for( int ci = _ci1; ci <= _ci2; ++ci )
//            {
//                auto c = &parent->cells[ ri * parent->columnCount + ci ];
//                bool found = false;
//                for( int i = 0; i < cells.Size(); ++i )
//                {
//                    if( cells[ i ].first == c )
//                    {
//                        found = true;
//                        break;
//                    }
//                }
//                if( found ) continue;
//                cells.Push( std::make_pair( c, c->items.Insert( this ) ) );
//            }
//        }
//    }
//
//    int CdItem::GetNearItems( List<CdItem*>& _container )
//    {
//        parent->IncreaseFlag();
//        _container.Clear();
//        flag = parent->autoFlag;     // 防止目标 item 被加入集合
//        for( int i = 0; i < cells.Size(); ++i )
//        {
//            auto& cis = cells[ i ].first->items;
//            for( int j = 0; j < cis.Size(); ++j )
//            {
//                auto& item = cis[ j ]->value;
//                if( item->flag == flag ) continue;
//                item->flag = flag;
//                _container.Push( item );
//            }
//        }
//        return _container.Size();
//    }
//    int CdItem::GetCollisionItems( List<CdItem*>& _container )
//    {
//        parent->IncreaseFlag();
//        _container.Clear();
//        flag = parent->autoFlag;     // 防止目标 item 被加入集合
//        for( int i = 0; i < cells.Size(); ++i )
//        {
//            auto& cis = cells[ i ].first->items;
//            for( int j = 0; j < cis.Size(); ++j )
//            {
//                auto& item = cis[ j ]->value;
//                if( item->flag == flag ) continue;
//
//                if( ( groupId & item->groupId ) != 0 )
//                {
//                    if( groupId == item->groupId ) continue;
//                }
//                if( ( layerMask & item->layerMask ) == 0 ) continue;
//
//                if( !CdGrid::CheckCollision( this, item ) ) continue;
//
//                item->flag = flag;
//                _container.Push( item );
//            }
//        }
//        return _container.Size();
//    }
//
//    // 从上面的函数精简而来
//    CdItem* CdItem::GetCollisionItem()
//    {
//        for( int i = 0; i < cells.Size(); ++i )
//        {
//            auto& cis = cells[ i ].first->items;
//            for( int j = 0; j < cis.Size(); ++j )
//            {
//                auto& item = cis[ j ]->value;
//                if( item == this ) continue;
//
//                if( ( groupId & item->groupId ) != 0 )
//                {
//                    if( groupId == item->groupId ) continue;
//                }
//                if( ( layerMask & item->layerMask ) == 0 ) continue;
//
//                if( !CdGrid::CheckCollision( this, item ) ) continue;
//
//                return item;
//            }
//        }
//        return nullptr;
//    }
//
//}
