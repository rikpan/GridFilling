#include <iostream>
#include <list>
#include <set>
#include <queue>

// 顺时针旋转
enum class Rotation
{
    _0,
    _90,
    _180,
    _270,
};

class GridGroup;

class Grid
{
public:
    Grid()
    {
    }

    void SetGridGroup(GridGroup* gridGroup)
    {
        m_GridGroup = gridGroup;
    }

    GridGroup* GetGridGroup() const
    {
        return m_GridGroup;
    }

private:
    GridGroup* m_GridGroup = NULL; // 所属格子组
};

// 格子组
class GridGroup
{
public:
    GridGroup()
    {
    }
    
    GridGroup(int width, int height)
    {
        SetSize(width, height);
    }

    ~GridGroup()
    {
        Destroy();
    }

    void Destroy()
    {
        if (m_Grids != NULL)
        {
            for (auto i = 0; i < m_Width; i++)
            {
                delete[] m_Grids[i];
            }
            delete[] m_Grids;
        }
        m_Grids = NULL;
        m_Width = 0;
        m_Height = 0;
    }

    int GetWidth() const
    {
        return m_Width;
    }

    int GetHeight() const
    {
        return m_Height;
    }

    bool SetGrid(int x, int y, Grid* grid)
    {
        if (grid != NULL)
        {
            if (x >= m_Width || y >= m_Height)
            {
                SetSize(x + 1, y + 1);
            }

            if (m_Grids[x][y] == NULL)
            {
                m_Grids[x][y] = grid;
                grid->SetGridGroup(this);
                return true;

            }
            else
                return false;
        }
        else
        {
            if (x < m_Width && y < m_Height)
            {
                m_Grids[x][y] = NULL;
                return true;

            }
            else
                return false;
        }
    }

    Grid* GetGrid(int x, int y) const
    {
        if (x < m_Width && y < m_Height)
            return m_Grids[x][y];
        else
            return NULL;
    }

    Grid*** GetGrids() const
    {
        return m_Grids;
    }

    void SetMoveable(bool v)
    {
        m_Moveable = v;
    }

    bool GetMoveable() const
    {
        return m_Moveable;
    }

    void SetRotatable(bool v)
    {
        m_Rotatable = v;
    }

    bool GetRotatable() const
    {
        return m_Rotatable;
    }

private:
    int m_Width = 0; // 原始宽
    int m_Height = 0; // 原始高
    Grid*** m_Grids = NULL; // 格子
    bool m_Moveable = true; // 是否可移动
    bool m_Rotatable = true; // 是否可旋转

    // width和height只能扩大，不能缩小
    void SetSize(int newWidth, int newHeight)
    {
        if (newWidth > m_Width || newHeight > m_Height)
        {
            if (newWidth < m_Width)
                newWidth = m_Width;
            if (newHeight < m_Height)
                newHeight = m_Height;
            auto newGrids = new Grid**[newWidth];
            for (auto i = 0; i < newWidth; i++)
            {
                newGrids[i] = new Grid*[newHeight];
                if (m_Height > 0)
                    std::memcpy(&newGrids[i], &m_Grids[i], m_Height * sizeof(Grid*));
            }
            Destroy();
            m_Grids = newGrids;
            m_Width = newWidth;
            m_Height = newHeight;
        }
    }
};

class GridGroupTransform
{
public:
    GridGroupTransform(GridGroup* gridGroup)
    {
        m_GridGroup = gridGroup;
    }

    ~GridGroupTransform()
    {
    }

    void SetPos(int posX, int posY)
    {
        m_PosX = posX;
        m_PosX = posY;
    }

    int GetPosX() const
    {
        return m_PosX;
    }

    int GetPosY() const
    {
        return m_PosY;
    }

    void SetRotate(Rotation r)
    {
        m_Rotation = r;
    }

    Rotation GetRotate() const
    {
        return m_Rotation;
    }

    GridGroup* GetGridGroup() const
    {
        return m_GridGroup;
    }

    GridGroupTransform* Clone()
    {
        auto clone = new GridGroupTransform(m_GridGroup);
        clone->m_PosX = m_PosX;
        clone->m_PosY = m_PosY;
        clone->m_Rotation = m_Rotation;
        return clone;
    }

    int GetWidth() const
    {
        if (m_Rotation == Rotation::_0 || m_Rotation == Rotation::_180)
            return m_GridGroup->GetWidth();
        else
            return m_GridGroup->GetHeight();
    }

    int GetHeight() const
    {
        if (m_Rotation == Rotation::_0 || m_Rotation == Rotation::_180)
            return m_GridGroup->GetHeight();
        else
            return m_GridGroup->GetWidth();
    }

    Grid* GetGrid(int posX, int posY) const
    {
        auto x = posX - m_PosX;
        auto y = posY - m_PosY;
        if (x >= 0 && x < GetWidth() && y >= 0 && y < GetHeight())
        {
            switch (m_Rotation)
            {
            case Rotation::_0:
                return m_GridGroup->GetGrid(x, y);
            case Rotation::_90:
                return m_GridGroup->GetGrid(y, m_GridGroup->GetWidth() - x);
            case Rotation::_180:
                return m_GridGroup->GetGrid(m_GridGroup->GetWidth() - x, m_GridGroup->GetHeight() - y);
            case Rotation::_270:
                return m_GridGroup->GetGrid(m_GridGroup->GetHeight() - y, x);
            }
        }
        else
            return NULL;
    }

    // 判断trans是否与本transform的格子重叠
    bool IsOverlapTransform(GridGroupTransform* trans) const
    {
        for (auto x = trans->GetPosX(); x < trans->GetPosX() + trans->GetWidth(); x++)
        {
            for (auto y = trans->GetPosY(); y < trans->GetPosY() + trans->GetHeight(); y++)
            {
                if (trans->GetGrid(x, y) != NULL && GetGrid(x, y) != NULL)
                    // 这个位置已经有其他格子了，失败
                    return false;
            }
        }
    }

    // 判断trans是否与本transform的格子相邻
    bool IsTouchTransform(GridGroupTransform* trans) const
    {
        for (auto x = trans->GetPosX(); x < trans->GetPosX() + trans->GetWidth(); x++)
        {
            for (auto y = trans->GetPosY(); y < trans->GetPosY() + trans->GetHeight(); y++)
            {
                // 这个位置自己有格子，本transform没有格子，且上下左右有格子，那么就算是相邻
                if (trans->GetGrid(x, y) != NULL &&
                    GetGrid(x, y) != NULL &&
                    (GetGrid(x - 1, y) != NULL ||
                     GetGrid(x + 1, y) != NULL ||
                     GetGrid(x, y + 1) != NULL ||
                     GetGrid(x, y - 1) != NULL))
                {
                    return true;
                }
            }
        }
        return false;
    }

    bool TryAddTransform(GridGroupTransform* trans)
    {
        if (m_Children.find(trans) != m_Children.end())
            return true;

        //for (auto x = trans->GetPosX(); x < trans->GetPosX() + trans->GetWidth(); x++)
        //{
        //    for (auto y = trans->GetPosY(); y < trans->GetPosY() + trans->GetHeight(); y++)
        //    {
        //        auto grid = GetGrid(x, y);
        //        if (grid != NULL)
        //            // 这个位置放不下，失败
        //            return false;
        //    }
        //}

        // 将trans放入
        for (auto x = trans->GetPosX(); x < trans->GetPosX() + trans->GetWidth(); x++)
        {
            for (auto y = trans->GetPosY(); y < trans->GetPosY() + trans->GetHeight(); y++)
            {
                auto grid = trans->GetGrid(x, y);
                if (grid != NULL)
                    // 这个位置有格子了，失败
                    return false;
                m_GridGroup->SetGrid(x, y, grid);
            }
        }

        m_Children.insert(trans);

        return true;
    }

    bool TryRemoveTransform(GridGroupTransform* trans)
    {
        auto it = m_Children.find(trans);
        if (it == m_Children.end())
            return false;
        m_Children.erase(it);

        // 将trans移除
        for (auto x = 0; x < m_GridGroup->GetWidth(); x++)
        {
            for (auto y = 0; y < m_GridGroup->GetHeight(); y++)
            {
                auto grid = m_GridGroup->GetGrid(x, y);
                if (grid != NULL && grid->GetGridGroup() == trans->GetGridGroup())
                    m_GridGroup->SetGrid(x, y, NULL);
            }
        }

        return true;
    }

private:
    // 原始数据
    GridGroup* m_GridGroup = NULL;
    int m_PosX = 0, m_PosY = 0; // 局部位置，左下角
    Rotation m_Rotation = Rotation::_0; // 旋转
    std::set<GridGroupTransform*> m_Children; // 子节点
};

class Item
{
public:
    Item(int classId)
    {
        m_ClassId = classId;
        m_GridGroup = new GridGroup();
    }

    ~Item()
    {
        if (m_GridGroup != NULL)
            delete m_GridGroup;
        m_GridGroup = NULL;
    }

    int GetClassId() const
    {
        return m_ClassId;
    }

    GridGroup* GetGridGroup() const
    {
        return m_GridGroup;
    }

private:
    int m_ClassId;
    GridGroup* m_GridGroup = NULL;
};

class Bag
{
public:
    Bag(int classId, int fixedPosX, int fixedPosY)
    {
        m_ClassId = classId;
        m_GridGroup = new GridGroup();
        m_FixedPosX = fixedPosX;
        m_FixedPosY = fixedPosY;
        m_GridGroup->SetMoveable(m_FixedPosX < 0 || m_FixedPosY < 0);
        m_GridGroup->SetRotatable(m_FixedPosX < 0 || m_FixedPosY < 0);
    }

    ~Bag()
    {
        if (m_GridGroup != NULL)
            delete m_GridGroup;
        m_GridGroup = NULL;
    }

    int GetClassId() const
    {
        return m_ClassId;
    }

    GridGroup* GetGridGroup() const
    {
        return m_GridGroup;
    }

    int GetFixedPosX() const
    {
        return m_FixedPosX;
    }

    int GetFixedPosY() const
    {
        return m_FixedPosY;
    }

private:
    int m_ClassId;
    GridGroup* m_GridGroup = NULL;
    // 左下角固定位置，如果两个都小于0，则可移动，可旋转
    int m_FixedPosX = -1, m_FixedPosY = -1;
};

class Backpack : public GridGroup
{
public:
    Backpack(int width, int height)
    {
        m_GridGroup = new GridGroup(width, height);
    }

    ~Backpack()
    {
        if (m_GridGroup != NULL)
            delete m_GridGroup;
        m_GridGroup = NULL;
    }

    GridGroup* GetGridGroup() const
    {
        return m_GridGroup;
    }

private:
    GridGroup* m_GridGroup = NULL;
};

class BackpackLayout
{
public:
    BackpackLayout(Backpack* backpack, Bag** bags, int bagCount, Item** items, int itemCount)
    {
        m_Backpack = backpack;
        m_Bags = bags;
        m_BagCount = bagCount;
        m_Items = items;
        m_ItemCount = itemCount;
        for (int i = 0; i < m_BagCount; i++)
        {
            auto bag = m_Bags[i];
            if (!bag->GetGridGroup()->GetMoveable())
            {
                // 不可移动背包
                m_FixedBags.push_back(bag);
            }
            else
            {
                // 可移动背包
                m_MovableBags.push_back(bag);

            }
        }
    }

    ~BackpackLayout()
    {
        if (m_Backpack != NULL)
            delete m_Backpack;
        m_Backpack = NULL;

        if (m_Bags != NULL)
        {
            for (int i = 0; i < m_BagCount; i++)
            {
                if (m_Bags[i] != NULL)
                    delete m_Bags[i];
                m_Bags[i] = NULL;
            }
            delete[] m_Bags;
            m_Bags = NULL;
        }

        if (m_Items != NULL)
        {
            for (int i = 0; i < m_ItemCount; i++)
            {
                if (m_Items[i] != NULL)
                    delete m_Items[i];
                m_Items[i] = NULL;
            }
            delete[] m_Items;
            m_Items = NULL;
        }

        if (m_BackpackTrans != NULL)
            delete m_BackpackTrans;
        m_BackpackTrans = NULL;
    }

    void Layout()
    {
        // 构造背包
        if (m_BackpackTrans != NULL)
            delete m_BackpackTrans;
        m_BackpackTrans = new GridGroupTransform(m_Backpack->GetGridGroup());

        // 优先放置不能移动的背包
        for (auto bag : m_FixedBags)
        {
            auto trans = new GridGroupTransform(bag->GetGridGroup());
            trans->SetPos(bag->GetFixedPosX(), bag->GetFixedPosY());

            // 尝试放入背包
            if (!m_BackpackTrans->TryAddTransform(trans))
            {
                // 这个位置不能放下，失败
                throw std::exception("固定背包放置失败!");
            }
        }

        // 开始递归放置可以移动的背包
        LayoutBag(0);
    }

    void LayoutBag(int bagIdx)
    {
        if (bagIdx >= (int)m_MovableBags.size())
        {
            // 所有背包都放好了
            return;
        }

        auto bag = m_MovableBags[bagIdx];
        auto bagTrans = new GridGroupTransform(bag->GetGridGroup());
        // 遍历背包能放入的所有位置，判断是否可以放入
        for (int x = 0; x < m_BackpackTrans->GetWidth() - bagTrans->GetWidth(); x++)
        {
            for (int y = 0; y < m_BackpackTrans->GetHeight() - bagTrans->GetHeight(); y++)
            {
                bagTrans->SetPos(x, y);
                if (!m_BackpackTrans->IsOverlapTransform(bagTrans) &&
                    m_BackpackTrans->IsTouchTransform(bagTrans))
                {
                    if (!m_BackpackTrans->TryAddTransform(bagTrans))
                    {
                        delete bagTrans;
                        throw std::exception("可移动背包放置失败!");
                    }
                }
            }
        }
    }

private:
    Backpack* m_Backpack = NULL;
    Bag** m_Bags = NULL;
    int m_BagCount = 0;
    std::vector<Bag*> m_MovableBags;
    std::vector<Bag*> m_FixedBags;
    Item** m_Items = NULL;
    int m_ItemCount = 0;

    GridGroupTransform* m_BackpackTrans = NULL;
};

int main()
{
    auto backpack = new Backpack(10, 10);
    delete backpack;
}
