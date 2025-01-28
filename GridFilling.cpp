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

// 迫于代码简单易懂的考量，不统一使用Grid，改为将GridGroup模板化

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

    bool AddGrid(int x, int y, Grid* grid)
    {
        if (SetGrid(x, y, grid))
        {
            grid->SetGridGroup(this);
            return true;
        }
        else
            return false;
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
                m_RecheckRotationValid = true;
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
                m_RecheckRotationValid = true;
                return true;

            }
            else
                return false;
        }
    }

    // 顺时针旋转
    void GetRotationPos(int x, int y, Rotation r, int* rx, int* ry) const
    {
        if (r == Rotation::_0)
        {
            *rx = x;
            *ry = y;
        }
        else if (r == Rotation::_90)
        {
            *rx = y;
            *ry = m_Width - x - 1;
        }
        else if (r == Rotation::_180)
        {
            *rx = m_Width - x - 1;
            *ry = m_Height - y - 1;
        }
        else if (r == Rotation::_270)
        {
            *rx = m_Height - y - 1;
            *ry = x;
        }
    }

    // 逆时针旋转
    void GetInvRotationPos(int x, int y, Rotation r, int* rx, int* ry) const
    {
        if (r == Rotation::_0)
        {
            GetRotationPos(x, y, Rotation::_0, rx, ry);
        }
        else if (r == Rotation::_90)
        {
            GetRotationPos(x, y, Rotation::_270, rx, ry);
        }
        else if (r == Rotation::_180)
        {
            GetRotationPos(x, y, Rotation::_180, rx, ry);
        }
        else if (r == Rotation::_270)
        {
            GetRotationPos(x, y, Rotation::_90, rx, ry);
        }
    }

    Grid* GetGrid(int x, int y) const
    {
        if (x >= 0 && x < m_Width && y >= 0 && y < m_Height)
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

    void CheckRotationValid()
    {
        m_RotationValid[0] = true; // 0旋转一定有效
        for (int x = 0; x < m_Width; x++)
        {
            for (int y = 0; y < m_Height; y++)
            {
                int _90x, _90y;
                GetRotationPos(x, y, Rotation::_90, &_90x, &_90y);
                int _180x, _180y;
                GetRotationPos(x, y, Rotation::_180, &_180x, &_180y);
                if (GetGrid(x, y) != NULL)
                {
                    m_RotationValid[1] |= GetGrid(_90x, _90y) == NULL; // x, y有grid，但90旋转的位置没有grid，那么就需要90旋转
                    m_RotationValid[2] |= GetGrid(_180x, _180y) == NULL; // x, y有grid，但180旋转的位置没有grid，那么就需要180旋转
                }
                else
                {
                    m_RotationValid[1] |= GetGrid(_90x, _90y) != NULL; // x, y没有grid，但90旋转的位置有grid，那么就需要90旋转
                    m_RotationValid[2] |= GetGrid(_180x, _180y) != NULL; // x, y没有grid，但180旋转的位置有grid，那么就需要180旋转
                }
                m_RotationValid[3] = m_RotationValid[2]; // 如果旋转180有必要，那么270也有必要，因为0到180，等价于90到270

                if (m_RotationValid[0] && m_RotationValid[1] && m_RotationValid[2] && m_RotationValid[3])
                    return; // 全部有必要，返回
            }
        }
    }

    const bool* GetRotationValid()
    {
        if (m_RecheckRotationValid)
        {
            m_RecheckRotationValid = false;
            CheckRotationValid();
        }
        return m_RotationValid;
    }

private:
    int m_Width = 0; // 原始宽
    int m_Height = 0; // 原始高
    Grid*** m_Grids = NULL; // 格子
    bool m_Moveable = true; // 是否可移动
    bool m_Rotatable = true; // 是否可旋转
    bool m_RecheckRotationValid = false; // 是否需要重新检查旋转有效性
    bool m_RotationValid[4] = {true, true, true, true}; // 0, 90, 180, 270旋转是否有意义; 0一定为true

    // width和height只能扩大，不能缩小
    void SetSize(int newWidth, int newHeight)
    {
        if (newWidth > m_Width || newHeight > m_Height)
        {
            if (newWidth < m_Width)
                newWidth = m_Width;
            if (newHeight < m_Height)
                newHeight = m_Height;
            auto newGrids = new Grid**[newWidth]{ NULL };
            for (auto i = 0; i < newWidth; i++)
                newGrids[i] = new Grid*[newHeight]{ NULL };
            if (m_Width * m_Height > 0)
            {
                for (auto i = 0; i < m_Width; i++)
                    std::memcpy(newGrids[i], m_Grids[i], m_Height * sizeof(Grid*));
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
        GenDebugString();
    }

    ~GridGroupTransform()
    {
    }

    void SetPos(int posX, int posY)
    {
        m_PosX = posX;
        m_PosY = posY;
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
        GenDebugString();
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

    // posX和posY是世界坐标，因此下面要做逆移动和逆旋转
    Grid* GetGrid(int worldPosX, int worldPosY) const
    {
        int x, y;
        m_GridGroup->GetInvRotationPos(worldPosX - m_PosX, worldPosY - m_PosY, m_Rotation, &x, &y);
        return m_GridGroup->GetGrid(x, y);
    }

    // posX和posY是世界坐标，因此下面要做逆移动和逆旋转
    Grid* GetGridLocal(int localPosX, int localPosY) const
    {
        int x, y;
        m_GridGroup->GetInvRotationPos(localPosX, localPosY, m_Rotation, &x, &y);
        return m_GridGroup->GetGrid(x, y);
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
                    return true;
            }
        }
        return false;
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
                    GetGrid(x, y) == NULL &&
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
                {
                    // 这个位置有格子了，失败
                    if (m_GridGroup->GetGrid(x, y) != NULL)
                        return false;
                    m_GridGroup->SetGrid(x, y, grid);
                }
            }
        }

        m_Children.insert(trans);

        GenDebugString();

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

        GenDebugString();

        return true;
    }

    void GenDebugString()
    {
        m_DebugString.clear();
        for (auto y = GetHeight() - 1; y >= 0; y--)
        {
            for (auto x = 0; x < GetWidth(); x++)
            {
                if (GetGridLocal(x, y) == NULL)
                    m_DebugString += " × ";
                else
                    m_DebugString += " ● ";
            }
            m_DebugString += "\n";
        }
    }

private:
    GridGroup* m_GridGroup = NULL; // 原始数据
    int m_PosX = 0, m_PosY = 0; // 局部位置，左下角
    Rotation m_Rotation = Rotation::_0; // 旋转
    std::set<GridGroupTransform*> m_Children; // 子节点
    std::string m_DebugString; // 调试用字符串
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
    Bag(int classId, int fixedPosX = -1, int fixedPosY = -1)
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
    BackpackLayout()
    {
    }

    ~BackpackLayout()
    {
        if (m_Backpack != NULL)
            delete m_Backpack;
        m_Backpack = NULL;

        for (auto bag : m_Bags)
        {
            if (bag != NULL)
                delete bag;
        }

        for (auto item : m_Items)
        {
            if (item != NULL)
                delete item;
        }

        if (m_BackpackTrans != NULL)
            delete m_BackpackTrans;
        m_BackpackTrans = NULL;
    }

    void SetBackpack(Backpack* backpack)
    {
        if (m_Backpack != NULL)
            delete m_Backpack;
        m_Backpack = backpack;
    }

    void AddBag(Bag* bag)
    {
        m_Bags.push_back(bag);
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

    void AddItem(Item* item)
    {
        m_Items.push_back(item);
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

    // 注意，这里没必要在穷举所有背包的顺序，只需要顺序放入即可，因为二者是等价的
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
                // 设置位置
                bagTrans->SetPos(x, y);

                // 遍历所有旋转情况
                for (int r = 0; r < 4; r++)
                {
                    if (bagTrans->GetGridGroup()->GetRotationValid()[r])
                    {
                        // 设置旋转
                        bagTrans->SetRotate((Rotation)r);

                        // 必须不与本Transform重叠，且必须与本Transform相邻
                        if (!m_BackpackTrans->IsOverlapTransform(bagTrans) &&
                            m_BackpackTrans->IsTouchTransform(bagTrans))
                        {
                            // 尝试添加，在有上面两个判断下，这里不可能失败
                            if (!m_BackpackTrans->TryAddTransform(bagTrans))
                            {
                                delete bagTrans;
                                throw std::exception("可移动背包放置失败!");
                            }

                            // 继续放置下一个背包
                            LayoutBag(bagIdx + 1);

                            // 移除再继续
                            if (!m_BackpackTrans->TryRemoveTransform(bagTrans))
                            {
                                delete bagTrans;
                                throw std::exception("可移动背包移除失败!");
                            }
                        }
                    }
                }
            }
        }
    }

private:
    Backpack* m_Backpack = NULL;
    std::vector<Bag*> m_Bags;
    std::vector<Bag*> m_MovableBags;
    std::vector<Bag*> m_FixedBags;
    std::vector<Item*> m_Items;

    GridGroupTransform* m_BackpackTrans = NULL;
};

int main()
{
    BackpackLayout backpackLayout;

    auto backpack = new Backpack(10, 10);
    backpackLayout.SetBackpack(backpack);

    auto bag1 = new Bag(1, 0, 0);
    bag1->GetGridGroup()->AddGrid(0, 0, new Grid());
    bag1->GetGridGroup()->AddGrid(0, 1, new Grid());
    bag1->GetGridGroup()->AddGrid(1, 0, new Grid());
    bag1->GetGridGroup()->AddGrid(1, 1, new Grid());
    backpackLayout.AddBag(bag1);

    auto bag2 = new Bag(2);
    bag2->GetGridGroup()->AddGrid(0, 0, new Grid());
    bag2->GetGridGroup()->AddGrid(0, 1, new Grid());
    bag2->GetGridGroup()->AddGrid(1, 0, new Grid());
    backpackLayout.AddBag(bag2);

    



    backpackLayout.Layout();
}
