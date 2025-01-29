#include <iostream>
#include <list>
#include <set>
#include <queue>
#include <sstream>
#include <chrono>

// #define DEBUG_STRING

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

    ~Grid()
    {
        //if (m_SubGrid != NULL)
        //    throw new std::exception("子格子不为空");
    }

    void SetOwner(GridGroup* gridGroup)
    {
        m_Owner = gridGroup;
    }

    GridGroup* GetOwner() const
    {
        return m_Owner;
    }

    void SetSubGrid(Grid* grid)
    {
        m_SubGrid = grid;
    }

    Grid* GetSubGrid() const
    {
        return m_SubGrid;
    }

private:
    Grid* m_SubGrid = NULL; // 所属放置的格子
    GridGroup* m_Owner = NULL; // 所属格子组
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

    void SetId(int id)
    {
        m_Id = id;
    }

    int GetId() const
    {
        return m_Id;
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
                grid->SetOwner(this);
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
        else
        {
            std::stringstream ss;
            ss << "非法旋转: " << (int)r;
            std::string formatted = ss.str();
            throw new std::exception(formatted.c_str());
        }
    }

    // 逆时针旋转
    void GetInvRotationPos(int x, int y, Rotation r, int* rx, int* ry) const
    {
        if (r == Rotation::_0)
        {
            *rx = x;
            *ry = y;
        }
        else if (r == Rotation::_90)
        {
            *rx = y;
            *ry = m_Height - x - 1;
        }
        else if (r == Rotation::_180)
        {
            *rx = m_Height - x - 1;
            *ry = m_Width - y - 1;
        }
        else if (r == Rotation::_270)
        {
            *rx = m_Width - y - 1;
            *ry = x;
        }
        else
        {
            std::stringstream ss;
            ss << "非法旋转: " << (int)r;
            std::string formatted = ss.str();
            throw new std::exception(formatted.c_str());
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
        m_RotationValid[1] = false;
        m_RotationValid[2] = false;
        m_RotationValid[3] = false;
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
    int m_Id = -1; // id
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
        RebuildDebugString();
        GenDebugString();
    }

    ~GridGroupTransform()
    {
        if (m_DebugString != NULL)
            delete m_DebugString;
        m_DebugString = NULL;
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
        RebuildDebugString();
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
    Grid* GetGridLocal(int lx, int ly) const
    {
        int x, y;
        m_GridGroup->GetInvRotationPos(lx, ly, m_Rotation, &x, &y);
        return m_GridGroup->GetGrid(x, y);
    }

    // 判断trans是否与本transform的格子重叠
    bool IsOverlapTransform(GridGroupTransform* trans) const
    {
        int transPosX = trans->GetPosX();
        int transPosY = trans->GetPosY();
        int transWidth = trans->GetGridGroup()->GetWidth();
        int transHeight = trans->GetGridGroup()->GetHeight();
        auto transGrids = trans->GetGridGroup()->GetGrids();

        int myPosX = GetPosX();
        int myPosY = GetPosY();
        int myWidth = GetWidth();
        int myHeight = GetHeight();
        auto myGrids = GetGridGroup()->GetGrids();
        int x, y;

        if (trans->GetRotate() == Rotation::_0)
        {
            for (auto _x = transPosX; _x < transPosX + transWidth; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transHeight; _y++)
                {
                    x = _x - transPosX;
                    y = _y - transPosY;
                    auto transGrid = transGrids[x][y];
                    //if (transGrid != trans->GetGrid(_x, _y))
                    //    return false;
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            //if (grid != GetGrid(_x, _y))
                            //    return false;
                            if (grid->GetSubGrid() != NULL)
                                // 这个位置已经有其他格子了，失败
                                return true;
                        }
                        else
                            return true;
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_90)
        {
            for (auto _x = transPosX; _x < transPosX + transHeight; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transWidth; _y++)
                {
                    x = _y - transPosY;
                    y = transHeight - (_x - transPosX) - 1;
                    auto transGrid = transGrids[x][y];
                    //if (transGrid != trans->GetGrid(_x, _y))
                    //    return false;
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            //if (grid != GetGrid(_x, _y))
                            //    return false;
                            if (grid->GetSubGrid() != NULL)
                                // 这个位置已经有其他格子了，失败
                                return true;
                        }
                        else
                            return true;
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_180)
        {
            for (auto _x = transPosX; _x < transPosX + transWidth; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transHeight; _y++)
                {
                    x = transHeight - (_x - transPosX) - 1;
                    y = transWidth - (_y - transPosY) - 1;
                    auto transGrid = transGrids[x][y];
                    //if (transGrid != trans->GetGrid(_x, _y))
                    //    return false;
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            //if (grid != GetGrid(_x, _y))
                            //    return false;
                            if (grid->GetSubGrid() != NULL)
                                // 这个位置已经有其他格子了，失败
                                return true;
                        }
                        else
                            return true;
                    }
                }
            }
        }
        else if (trans->GetRotate() == Rotation::_270)
        {
            for (auto _x = transPosX; _x < transPosX + transHeight; _x++)
            {
                for (auto _y = transPosY; _y < transPosY + transWidth; _y++)
                {
                    x = transWidth - (_y - transPosY) - 1;
                    y = _x - transPosX;
                    auto transGrid = transGrids[x][y];
                    //if (transGrid != trans->GetGrid(_x, _y))
                    //    return false;
                    if (transGrid != NULL)
                    {
                        x = _x - myPosX;
                        y = _y - myPosY;
                        if (x >= 0 && x < myWidth && y >= 0 && y < myHeight)
                        {
                            auto grid = myGrids[x][y];
                            //if (grid != GetGrid(_x, _y))
                            //    return false;
                            if (grid->GetSubGrid() != NULL)
                                // 这个位置已经有其他格子了，失败
                                return true;
                        }
                        else
                            return true;
                    }
                }
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
                if (trans->GetGrid(x, y) != NULL)
                {
                    auto grid = GetGrid(x, y);
                    if (grid != NULL && grid->GetSubGrid() == NULL)
                    {
                        auto gridLeft = GetGrid(x - 1, y);
                        if (gridLeft == NULL || gridLeft->GetSubGrid() == NULL)
                        {
                            auto gridRight = GetGrid(x + 1, y);
                            if (gridRight == NULL || gridRight->GetSubGrid() == NULL)
                            {
                                auto gridTop = GetGrid(x, y + 1);
                                if (gridTop == NULL || gridTop->GetSubGrid() == NULL)
                                {
                                    auto gridBottom = GetGrid(x, y - 1);
                                    if (gridBottom == NULL || gridBottom->GetSubGrid() == NULL)
                                        return false;
                                    else
                                        return true;
                                }
                                else
                                    return true;
                            }
                            else
                                return true;
                        }
                        else
                            return true;
                    }
                }
            }
        }
        return false;
    }

    bool TryAddTransform(GridGroupTransform* trans)
    {
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
                auto transGrid = trans->GetGrid(x, y);
                if (transGrid != NULL)
                {
                    auto grid = GetGrid(x, y);
                    // 这个位置不能放格子，或者已经被占据了，失败
                    if (grid == NULL || grid->GetSubGrid() != NULL)
                    {
                        TryRemoveTransform(trans);
                        return false;
                    }
                    grid->SetSubGrid(transGrid);
                }
            }
        }

        m_ChildrenCount++;

        GenDebugString();

        return true;
    }

    bool TryRemoveTransform(GridGroupTransform* trans)
    {
        if (m_ChildrenCount > 0)
        {
            // 将trans移除
            for (auto x = 0; x < m_GridGroup->GetWidth(); x++)
            {
                for (auto y = 0; y < m_GridGroup->GetHeight(); y++)
                {
                    auto grid = m_GridGroup->GetGrid(x, y);
                    if (grid != NULL)
                    {
                        auto subGrid = grid->GetSubGrid();
                        if (subGrid != NULL && subGrid->GetOwner() == trans->GetGridGroup())
                            grid->SetSubGrid(NULL);
                    }
                }
            }

            m_ChildrenCount--;

            GenDebugString();

            return true;
        }
        else
            return false;
    }

    //std::set<GridGroupTransform*>* GetChildren() const
    //{
    //    return m_Children;
    //}

    int GetChildrenCount() const
    {
        return m_ChildrenCount;
    }

    void RebuildDebugString()
    {
#ifdef DEBUG_STRING
        int size = GetHeight() * (GetWidth() * 3 + 1);
        if (m_DebugString != NULL)
            delete[] m_DebugString;
        m_DebugString = new char16_t[size + 1];
        m_DebugString[size] = '\0';

        int pos = 0;
        for (auto y = GetHeight() - 1; y >= 0; y--)
        {
            for (auto x = 0; x < GetWidth(); x++)
            {
                m_DebugString[pos++] = u' ';
                m_DebugString[pos++] = u' ';
                m_DebugString[pos++] = u' ';
            }
            m_DebugString[pos++] = u'\n';
        }
#endif
    }

    void GenDebugString()
    {
#ifdef DEBUG_STRING
        int pos = 0;
        for (auto y = GetHeight() - 1; y >= 0; y--)
        {
            for (auto x = 0; x < GetWidth(); x++)
            {
                pos++;
                auto grid = GetGridLocal(x, y);
                if (grid == NULL)
                    m_DebugString[pos++] = u'·';
                else if (grid->GetSubGrid() == NULL)
                    m_DebugString[pos++] = u'×';
                else
                {
                    auto owner = grid->GetSubGrid()->GetOwner();
                    if (owner != NULL && owner->GetId() >= 0)
                        m_DebugString[pos++] = u'0' + owner->GetId();
                    else
                        m_DebugString[pos++] = u'○';
                }
                pos++;
            }
            pos++;
        }
#endif
    }

private:
    GridGroup* m_GridGroup = NULL; // 原始数据
    int m_PosX = 0, m_PosY = 0; // 局部位置，左下角
    Rotation m_Rotation = Rotation::_0; // 旋转
    int m_ChildrenCount = 0;
    char16_t* m_DebugString; // 调试用字符串
};

class Item
{
public:
    Item(int classId)
    {
        m_ClassId = classId;
        m_GridGroup = new GridGroup();
        m_GridGroup->SetId(m_ClassId);
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
        m_GridGroup->SetId(m_ClassId);
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
        for (auto x = 0; x < width; x++)
        {
            for (auto y = 0; y < height; y++)
            {
                m_GridGroup->SetGrid(x, y, new Grid());
            }
        }
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
            // 不可移动包裹
            m_FixedBags.push_back(bag);
        }
        else
        {
            // 可移动包裹
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

        // 优先放置不能移动的包裹
        for (auto bag : m_FixedBags)
        {
            auto trans = new GridGroupTransform(bag->GetGridGroup());
            trans->SetPos(bag->GetFixedPosX(), bag->GetFixedPosY());

            // 尝试放入包裹
            if (!m_BackpackTrans->TryAddTransform(trans))
            {
                // 这个位置不能放下，失败
                throw std::exception("固定包裹放置失败!");
            }
        }

        // 递归包裹放置顺序
        std::vector<int> nums;
        for (int i = 0; i < (int)m_MovableBags.size(); ++i) 
        {
            nums.push_back(i);
        }
        LayoutBagSort(nums, 0, mBagSorts);

        // 开始递归放置可以移动的包裹
        for (int i = 0; i < (int)mBagSorts.size(); i++)
        {
            int maxWidth = 1, maxHeight = 1; // 第一个包裹，固定在0,0位置，也不允许移动
            LayoutBag(mBagSorts[i], maxWidth, maxHeight, 0);
        }
    }

    void LayoutBagSort(std::vector<int>& nums, int start, std::vector<std::vector<int>>& result)
    {
        if (start == (int)nums.size())
        {
            result.push_back(nums); // 找到一个排列，加入结果
            return;
        }

        for (int i = start; i < (int)nums.size(); ++i)
        {
            std::swap(nums[start], nums[i]); // 交换当前元素与起始元素
            LayoutBagSort(nums, start + 1, result); // 递归处理下一个位置
            std::swap(nums[start], nums[i]); // 恢复交换（回溯）
        }
    }

    void LayoutBag(std::vector<int>& bagSort, int& maxWidth, int& maxHeight, int bagIdx)
    {
        if (bagIdx >= (int)bagSort.size())
        {
            // 所有包裹都放好了
            m_LaytoutCount++;
            return;
        }

        auto bag = m_MovableBags[bagSort[bagIdx]];
        auto bagTrans = new GridGroupTransform(bag->GetGridGroup());
        // 遍历包裹能放入的所有位置，判断是否可以放入
        // 需要遍历的最大数值，是所有包裹的长宽之和
        for (int x = 0; x < maxWidth; x++)
        {
            for (int y = 0; y < maxHeight; y++)
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
                        if (m_BackpackTrans->GetChildrenCount() == 0 ||
                            !m_BackpackTrans->IsOverlapTransform(bagTrans) &&
                            m_BackpackTrans->IsTouchTransform(bagTrans))
                        {
                            // 尝试添加，在有上面两个判断下，这里不可能失败
                            if (!m_BackpackTrans->TryAddTransform(bagTrans))
                            {
                                delete bagTrans;
                                throw std::exception("可移动包裹放置失败!");
                            }

                            maxWidth += bagTrans->GetWidth();
                            maxHeight += bagTrans->GetHeight();

                            // 继续放置下一个包裹
                            LayoutBag(bagSort, maxWidth, maxHeight, bagIdx + 1);

                            maxWidth -= bagTrans->GetWidth();
                            maxHeight -= bagTrans->GetHeight();

                            // 移除再继续
                            if (!m_BackpackTrans->TryRemoveTransform(bagTrans))
                            {
                                delete bagTrans;
                                throw std::exception("可移动包裹移除失败!");
                            }
                        }
                    }
                }
            }
        }
        delete bagTrans;
    }

    int GetLaytoutCount() const
    {
        return m_LaytoutCount;
    }

private:
    Backpack* m_Backpack = NULL;
    std::vector<Bag*> m_Bags;
    std::vector<Bag*> m_MovableBags;
    std::vector<Bag*> m_FixedBags;
    std::vector<Item*> m_Items;

    GridGroupTransform* m_BackpackTrans = NULL;
    int m_LaytoutCount = 0;

    std::vector<std::vector<int>> mBagSorts;
};

int main()
{
    BackpackLayout backpackLayout;

    auto backpack = new Backpack(10, 10);
    backpackLayout.SetBackpack(backpack);

    {
        auto bag = new Bag(1);
        bag->GetGridGroup()->SetGrid(0, 0, new Grid());
        bag->GetGridGroup()->SetGrid(0, 1, new Grid());
        bag->GetGridGroup()->SetGrid(1, 0, new Grid());
        bag->GetGridGroup()->SetGrid(1, 1, new Grid());
        backpackLayout.AddBag(bag);
    }

    //{
    //    auto bag = new Bag(2);
    //    bag->GetGridGroup()->SetGrid(0, 0, new Grid());
    //    bag->GetGridGroup()->SetGrid(0, 1, new Grid());
    //    bag->GetGridGroup()->SetGrid(1, 0, new Grid());
    //    bag->GetGridGroup()->SetGrid(1, 1, new Grid());
    //    bag->GetGridGroup()->SetGrid(2, 1, new Grid());
    //    bag->GetGridGroup()->SetGrid(1, 2, new Grid());
    //    bag->GetGridGroup()->SetGrid(2, 2, new Grid());
    //    backpackLayout.AddBag(bag);
    //}

    {
        auto bag = new Bag(3);
        bag->GetGridGroup()->SetGrid(0, 0, new Grid());
        bag->GetGridGroup()->SetGrid(0, 1, new Grid());
        bag->GetGridGroup()->SetGrid(1, 0, new Grid());
        bag->GetGridGroup()->SetGrid(1, 1, new Grid());
        bag->GetGridGroup()->SetGrid(2, 1, new Grid());
        bag->GetGridGroup()->SetGrid(1, 2, new Grid());
        bag->GetGridGroup()->SetGrid(2, 2, new Grid());
        backpackLayout.AddBag(bag);
    }

    {
        auto bag = new Bag(4);
        bag->GetGridGroup()->SetGrid(0, 0, new Grid());
        bag->GetGridGroup()->SetGrid(0, 1, new Grid());
        bag->GetGridGroup()->SetGrid(0, 2, new Grid());
        bag->GetGridGroup()->SetGrid(1, 0, new Grid());
        bag->GetGridGroup()->SetGrid(1, 1, new Grid());
        bag->GetGridGroup()->SetGrid(1, 2, new Grid());
        bag->GetGridGroup()->SetGrid(2, 0, new Grid());
        bag->GetGridGroup()->SetGrid(2, 1, new Grid());
        bag->GetGridGroup()->SetGrid(2, 2, new Grid());
        backpackLayout.AddBag(bag);
    }

    {
        auto bag = new Bag(5);
        bag->GetGridGroup()->SetGrid(0, 0, new Grid());
        bag->GetGridGroup()->SetGrid(0, 1, new Grid());
        bag->GetGridGroup()->SetGrid(0, 2, new Grid());
        bag->GetGridGroup()->SetGrid(1, 0, new Grid());
        bag->GetGridGroup()->SetGrid(1, 1, new Grid());
        bag->GetGridGroup()->SetGrid(1, 2, new Grid());
        bag->GetGridGroup()->SetGrid(2, 0, new Grid());
        bag->GetGridGroup()->SetGrid(2, 1, new Grid());
        bag->GetGridGroup()->SetGrid(2, 2, new Grid());
        bag->GetGridGroup()->SetGrid(3, 0, new Grid());
        bag->GetGridGroup()->SetGrid(3, 1, new Grid());
        bag->GetGridGroup()->SetGrid(3, 2, new Grid());
        backpackLayout.AddBag(bag);
    }

    



    // 获取当前时间点
    auto now = std::chrono::system_clock::now();

    // 转换为毫秒
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto start_ms = now_ms.time_since_epoch().count();

    // 转换为微秒
    auto now_us = std::chrono::time_point_cast<std::chrono::microseconds>(now);
    auto start_us = now_us.time_since_epoch().count();

    backpackLayout.Layout();

    // 获取当前时间点
    now = std::chrono::system_clock::now();

    // 转换为毫秒
    now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto end_ms = now_ms.time_since_epoch().count();

    // 转换为微秒
    now_us = std::chrono::time_point_cast<std::chrono::microseconds>(now);
    auto end_us = now_us.time_since_epoch().count();

    // 显示当前毫秒
    std::cout << "Milliseconds: " << end_ms - start_ms << ", LayoutCount: " << backpackLayout.GetLaytoutCount() << std::endl;

    // 现实就消耗的微秒
    std::cout << "Microseconds: " << end_us - start_us << ", LayoutCount: " << backpackLayout.GetLaytoutCount() << std::endl;
}
