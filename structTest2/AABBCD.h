#pragma once
#include <vector>

namespace AABBCD {

	// CD = collision detection
	// 当前只支持矩形 AABB( 如果一个显示对象较为复杂，多注册几个Item即可 )

	struct Vec2 { int x, y; };
	struct Size { int w, h; };

	struct CdCell;
	struct CdGrid;

	struct CdItem {
		Size siz;															// 宽高
		Vec2 pos;															// 中心点坐标

		int groupId;                                                        // 0 组号将和其他组号发生碰撞，相同组号不发生
		uint32_t layerMask;                                                 // maskA & maskB 不为 0 的发生碰撞

		int ci1, ri1, ci2, ri2;                                             // 缓存 Index 操作结果之 左下 右上 位于的 cell 以加速 Update

		uint32_t flag;                                                      // 自增流水号，用于去重
		int idx;                                                            // 位于 CdGrid.items 的索引 for 快速 erase
		CdGrid* parent;                                                     // 所在 CdGrid 父容器( 同时也是 Init, Destroy 的识别 flag )

		std::vector<std::pair<CdCell*, Links<CdItem*>::Node*>> cells;       // 占据了哪些格子以及位于这些格子中的节点

		void* userData;

		CdItem();
		~CdItem();
		void Init(Size const& siz, Vec2 const& pos, int groupId = 0, uint32_t layerMask = 0xFFFFFFFFu, void* userData = nullptr);
		void Destroy();                                                     // 析构 item 并从 items 移动至 freeItems

		void Index();                                                       // 创建索引信息
		void Update(Vec2 const& _pos);										// 更新位置及索引信息
		int GetNearItems(std::vector<CdItem*>& _container);                 // 填充 _item 所占格子笼罩的 items, 返回个数
		int GetCollisionItems(std::vector<CdItem*>& _container);            // 填充与 _item 碰撞的 items, 返回个数
		CdItem* GetCollisionItem();                                         // 返回首个检测到的 与 _item 碰撞的 item
		// todo: 找离目标区域 最近的 item ? 得到范围内由近到远排列的 items list?
	};

	struct CdCell {
		int ci, ri;															// 列号，行号
		Links<CdItem*> items;                                               // 格内有哪些 item
	};

	// 格子阵列( 分割显示区域，综控 )
	struct CdGrid {
		std::vector<CdItem*> items;
		std::vector<CdItem*> freeItems;
		std::vector<CdCell> cells;

		Size gridDiameter;
		Size cellRadius;
		Size cellDiameter;
		int rowCount;
		int columnCount;

		uint32_t autoFlag;                                                  // 自增流水号，用于去重
		void IncreaseFlag();                                                // ++autoFlag; 循环后重置所有并跳过 0

		CdGrid();
		~CdGrid();

		void Init(Size _gridDiameter, int _rowCount, int _columnCount);		// 按一个总尺寸划分格子并 Clear
		void Reserve(int _capacity);										// 于 freeItems 中预创建 capacity 个 items
		void Clear();                                                       // Destroy items, 清 cells 的 items

		CdItem* CreateItem();                                               // 创建 CdItem 对象( 优先从 freeItems 拿 )，并返回( 需要自己 Init )

		int GetItems(std::vector<CdItem*>& _container, Vec2 const& _pos);   // 填充覆盖着某坐标的 items，返回个数
		CdItem* GetItem(Vec2 const& _pos);									// 返回覆盖着某坐标的，第1个扫到的 item
		static bool CheckCollision(CdItem* _a, CdItem* _b);					// 检查两个 item 是否相交
		static bool CheckCollision(CdItem* _a, Vec2 _pos);					// 检查 pos 是否位于 item 内
	};
}



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
//    void CdGrid::Init( Size _gridDiameter, int _rowCount, int _columnCount )
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
//        uint32_t r;
//        r = _a->siz.w + _b->siz.w;
//        if( (uint32_t)( _a->pos.x - _b->pos.x + r ) > r + r ) return false;
//        r = _a->siz.h + _b->siz.h;
//        if( (uint32_t)( _a->pos.y - _b->pos.y + r ) > r + r ) return false;
//        return true;
//    }
//
//    bool CdGrid::CheckCollision( CdItem* _a, Vec2 _pos )
//    {
//        return _a->pos.x - _a->siz.w <= _pos.x
//            && _a->pos.x + _a->siz.w >= _pos.x
//            && _a->pos.y - _a->siz.h <= _pos.y
//            && _a->pos.y + _a->siz.h >= _pos.y;
//    }
//
//    int CdGrid::GetItems( std::vector<CdItem*>& _container, Vec2 const& _pos )
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
//    CdItem* CdGrid::GetItem( Vec2 const& _pos )
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
//    void CdItem::Init( Size const& _radius, Vec2 const& _pos, int _groupId /*= 0*/, uint32_t _layerMask /*= 0xFFFFFFFFu*/, void* _userData /*= nullptr */ )
//    {
//        assert( parent );
//        siz = _radius;
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
//        Vec2 _p = { pos.x - siz.w, pos.y - siz.h };
//        int _ci1 = _p.x / parent->cellDiameter.w;
//        int _ri1 = _p.y / parent->cellDiameter.h;
//        assert( _ci1 >= 0 && _ci1 < parent->columnCount && _ri1 >= 0 && _ri1 < parent->rowCount );
//
//        _p = { pos.x + siz.w, pos.y + siz.h };
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
//    void CdItem::Update( Vec2 const& _pos )
//    {
//        pos = _pos;
//
//        Vec2 p = { _pos.x - siz.w, _pos.y - siz.h };
//        int _ci1 = p.x / parent->cellDiameter.w;
//        int _ri1 = p.y / parent->cellDiameter.h;
//        assert( _ci1 >= 0 && _ci1 < parent->columnCount && _ri1 >= 0 && _ri1 < parent->rowCount );
//
//        p = { _pos.x + siz.w, _pos.y + siz.h };
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
//    int CdItem::GetNearItems( std::vector<CdItem*>& _container )
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
//    int CdItem::GetCollisionItems( std::vector<CdItem*>& _container )
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
